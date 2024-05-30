/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/RegAlloc.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/Analysis.h"

#include <algorithm>
#include <queue>

using namespace cobra;

bool VirtualRegisterManager::isUsed(VirtualRegister r) {
  return !registers.test(r.getIndex());
}

bool VirtualRegisterManager::isFree(VirtualRegister r) {
  return !isUsed(r);
}

VirtualRegister VirtualRegisterManager::allocateRegister() {
  if (registers.none()) {
    // If all bits are set, create a new register and return it.
    unsigned numRegs = registers.size();
    registers.resize(numRegs + 1, false);
    VirtualRegister R = VirtualRegister{numRegs};
    assert(isUsed(R) && "Error allocating a new register.");
    return R;
  }
  
  // Search for a free register to use.
  unsigned i = registers.find_first();
  assert(i >= 0 && "Unexpected failure to allocate a register");
  VirtualRegister R = VirtualRegister{i};
  assert(isFree(R) && "Error finding a free register");
  registers.reset(i);
  
  return R;
}

void VirtualRegisterManager::killRegister(VirtualRegister reg) {
  assert(isUsed(reg) && "Killing an unused register!");
  registers.set(reg.getIndex());
  assert(isFree(reg) && "Error freeing register!");
}

bool VirtualRegisterAllocator::hasInstructionNumber(Instruction *I) {
  return instructionNumbers_.count(I);
}

unsigned VirtualRegisterAllocator::getInstructionNumber(Instruction *I) {
  auto it = instructionNumbers_.find(I);
  if (it != instructionNumbers_.end()) {
    return it->second;
  }
  
  instructionsByNumbers_.push_back(I);
  instructionInterval_.push_back(LiveInterval());
  
  unsigned newIdx = instructionsByNumbers_.size() - 1;
  instructionNumbers_[I] = newIdx;
  return newIdx;
}

static bool phiReadWrite(PhiInst *P) {
  bool localPhiUse = false;
  bool externalUse = false;
  bool terminatorUse = false;

  BasicBlock *parent = P->getParent();

  for (auto *U : P->getUsers()) {
    terminatorUse |= bool(dynamic_cast<TerminatorInst *>(U));
    localPhiUse |=
        (dynamic_cast<PhiInst *>(U) && U->getParent() == parent && P != U);
    externalUse |= U->getParent() != parent;
  }

  // TODO: the code used to perform a stricter check which missed some cases.
  // TODO: need to come up with a better condition.
  // bool localWrite = false;
  // for (int i = 0, limit = P->getNumEntries(); !localWrite && i < limit; ++i)
  // {
  //   auto entry = P->getEntry(i);
  //   localWrite |= entry.first != P && entry.second == parent;
  // }
  // return terminatorUse || localPhiUse || (externalUse && localWrite);

  return terminatorUse || localPhiUse || externalUse;
}

void VirtualRegisterAllocator::resolvePhis(std::vector<BasicBlock *> &order) {
  std::vector<PhiInst *> PHIs;
  IRBuilder builder(F);
  
  for (auto BB : order) {
    for (auto Inst : *BB) {
      if (auto *P = dynamic_cast<PhiInst *>(Inst)) {
        PHIs.push_back(P);
      }
    }
  }
  
  for (PhiInst *P : PHIs) {
    if (!phiReadWrite(P))
      continue;

    // The MOV sequence may clobber the PHI. Insert a copy.
    builder.setInsertionPoint(P->getParent()->getTerminator());
    auto *mov = builder.createMovInst(P);

    Value::UseListTy users = P->getUsers();
    // Update all external users:
    for (auto *U : users) {
      // Local uses of the PHI are allowed.
      if (!dynamic_cast<PhiInst *>(U) && !dynamic_cast<TerminatorInst *>(U) &&
          U->getParent() == P->getParent())
        continue;

      U->replaceFirstOperandWith(P, mov);
    }
  }
  
  std::map<Value *, MovInst *> copied;
  
  for (PhiInst *P : PHIs) {
    for (unsigned i = 0, e = P->getNumEntries(); i < e; ++i) {
      auto E = P->getEntry(i);
      auto *term = E.second->getTerminator();
      builder.setInsertionPoint(term);
      auto *mov = builder.createMovInst(E.first);
      P->updateEntry(i, mov, E.second);

      // If the terminator uses the value that we are inserting then we can fix
      // the lifetime by making it use the MOV. We can do this because we know
      // that terminators don't modify values in destination PHI nodes and this
      // allows us to merge the lifetime of the value and save a register.
      copied[E.first] = mov;
    }
  }
  
  // The terminator comes after the MOV sequence, so make sure it uses the
  // updated registers.
  for (auto &BB : order) {
    auto *term = BB->getTerminator();

    for (int i = 0, e = term->getNumOperands(); i < e; i++) {
      auto *op = term->getOperand(i);
      if (dynamic_cast<Literal *>(op))
        continue;
      auto it = copied.find(op);
      if (it != copied.end()) {
        if (it->second->getParent() == BB) {
          term->setOperand(it->second, i);
          it->second->moveBefore(term);
        }
      }
    }
  }
}

void VirtualRegisterAllocator::calculateLocalLiveness(
    BlockLifetimeInfo &livenessInfo,
    BasicBlock *BB) {
  for (auto &it : *BB) {
    Instruction *I = it;

    unsigned idx = getInstructionNumber(I);
    livenessInfo.kill_.set(idx);

    // PHI nodes require special handling because they are flow sensitive. Mask
    // out flow that does not go in the direction of the phi edge.
    if (auto *P = dynamic_cast<PhiInst *>(I)) {
      std::vector<unsigned> incomingValueNum;

      // Collect all incoming value numbers.
      for (int i = 0, e = P->getNumEntries(); i < e; i++) {
        auto E = P->getEntry(i);
        // Skip unreachable predecessors.
        if (!blockLiveness_.count(E.second))
          continue;
        if (auto *II = dynamic_cast<Instruction *>(E.first)) {
          incomingValueNum.push_back(getInstructionNumber(II));
        }
      }

      // Block the incoming values from flowing into the predecessors.
      for (int i = 0, e = P->getNumEntries(); i < e; i++) {
        auto E = P->getEntry(i);
        // Skip unreachable predecessors.
        if (!blockLiveness_.count(E.second))
          continue;
        for (auto num : incomingValueNum) {
          blockLiveness_[E.second].maskIn_.set(num);
        }
      }

      // Allow the flow of incoming values in specific directions:
      for (int i = 0, e = P->getNumEntries(); i < e; i++) {
        auto E = P->getEntry(i);
        // Skip unreachable predecessors.
        if (!blockLiveness_.count(E.second))
          continue;
        if (auto *II = dynamic_cast<Instruction *>(E.first)) {
          unsigned idxII = getInstructionNumber(II);
          blockLiveness_[E.second].maskIn_.reset(idxII);
        }
      }
    }

    // For each one of the operands that are also instructions:
    for (unsigned opIdx = 0, e = I->getNumOperands(); opIdx != e; ++opIdx) {
      auto *opInst = dynamic_cast<Instruction *>(I->getOperand(opIdx));
      if (!opInst)
        continue;
      // Skip instructions from unreachable blocks.
      if (!blockLiveness_.count(opInst->getParent()))
        continue;

      // Get the index of the operand.
      auto opInstIdx = getInstructionNumber(opInst);
      livenessInfo.gen_.set(opInstIdx);
    }
  }
  
}

void VirtualRegisterAllocator::calculateGlobalLiveness(std::vector<BasicBlock *> &order) {
  unsigned iterations = 0;
  bool changed = false;

  // Init the live-in vector to be GEN-KILL.
  for (auto &it : blockLiveness_) {
    BlockLifetimeInfo &livenessInfo = it.second;
    livenessInfo.liveIn_ |= livenessInfo.gen_;
    livenessInfo.liveIn_.reset(livenessInfo.kill_);
    livenessInfo.liveIn_.reset(livenessInfo.maskIn_);
  }

  do {
    iterations++;
    changed = false;

    for (auto it = order.rbegin(), e = order.rend(); it != e; it++) {
      BasicBlock *BB = *it;
      BlockLifetimeInfo &livenessInfo = blockLiveness_[BB];

      // Rule:  OUT = SUCC0_in  + SUCC1_in ...
      for (auto *succ : successors(BB)) {
        BlockLifetimeInfo &succInfo = blockLiveness_[succ];
        // Check if we are about to change bits in the live-out vector.
        if (succInfo.liveIn_.test(livenessInfo.liveOut_)) {
          changed = true;
        }

        livenessInfo.liveOut_ |= succInfo.liveIn_;
      }

      // Rule: In = gen + (OUT - KILL)
      // After initializing 'in' to 'gen' - 'kill', the only way for the result
      // to change is for 'out' to change, and we check if 'out' changes above.
      livenessInfo.liveIn_ = livenessInfo.liveOut_;
      livenessInfo.liveIn_ |= livenessInfo.gen_;
      livenessInfo.liveIn_.reset(livenessInfo.kill_);
      livenessInfo.liveIn_.reset(livenessInfo.maskIn_);
    }
  } while (changed);

  // Suppress -Wunused-but-set-variable warning with new compilers.
  (void)iterations;
}

void VirtualRegisterAllocator::calculateLiveIntervals(std::vector<BasicBlock *> &order) {
  /// Calculate the live intervals for each instruction. Start with a list of
  /// intervals that only contain the instruction itself.
  for (int i = 0, e = instructionsByNumbers_.size(); i < e; ++i) {
    // The instructions are ordered consecutively. The start offset of the
    // instruction is the index in the array plus one because the value starts
    // to live on the next instruction.
    instructionInterval_[i] = LiveInterval(i + 1, i + 1);
  }

  // For each basic block in the liveness map:
  for (BasicBlock *BB : order) {
    BlockLifetimeInfo &liveness = blockLiveness_[BB];

    auto startOffset = getInstructionNumber(*BB->begin());
    auto endOffset = getInstructionNumber(BB->getTerminator());

    // Register fly-through basic blocks (basic blocks where the value enters)
    // and leavs without doing anything to any of the operands.
    for (int i = 0, e = liveness.liveOut_.size(); i < e; i++) {
      bool leavs = liveness.liveOut_.test(i);
      bool enters = liveness.liveIn_.test(i);
      if (leavs && enters) {
        instructionInterval_[i].add(LiveRange(startOffset, endOffset + 1));
      }
    }

    // For each instruction in the block:
    for (auto &it : *BB) {
      auto instOffset = getInstructionNumber(it);
      // The instruction is defined in this basic block. Check if it is leaving
      // the basic block extend the interval until the end of the block.
      if (liveness.liveOut_.test(instOffset)) {
        instructionInterval_[instOffset].add(
            LiveRange(instOffset + 1, endOffset + 1));
        assert(
            !liveness.liveIn_.test(instOffset) &&
            "Livein but also killed in this block?");
      }

      // Extend the lifetime of the operands.
      for (int i = 0, e = it->getNumOperands(); i < e; i++) {
        auto instOp = dynamic_cast<Instruction *>(it->getOperand(i));
        if (!instOp)
          continue;

        if (!hasInstructionNumber(instOp)) {
          continue;
        }

        auto operandIdx = getInstructionNumber(instOp);
        // Extend the lifetime of the interval to reach this instruction.
        // Include this instruction in the interval in order to make sure that
        // the register is not freed before the use.

        auto start = operandIdx + 1;
        auto end = instOffset + 1;
        if (start < end) {
          auto r = LiveRange(operandIdx + 1, instOffset + 1);
          instructionInterval_[operandIdx].add(r);
        }
      }

      // Extend the lifetime of the PHI to include the source basic blocks.
      if (auto *P = dynamic_cast<PhiInst *>(it)) {
        for (int i = 0, e = P->getNumEntries(); i < e; i++) {
          auto E = P->getEntry(i);
          // PhiInsts may reference instructions from dead code blocks
          // (which will be unnumbered and unallocated). Since the edge
          // is necessarily also dead, we can just skip it.
          if (!hasInstructionNumber(E.second->getTerminator()))
            continue;

          unsigned termIdx = getInstructionNumber(E.second->getTerminator());
          LiveRange R(termIdx, termIdx + 1);
          instructionInterval_[instOffset].add(R);

          // Extend the lifetime of the predecessor to the end of the BB.
          if (auto *instOp = dynamic_cast<Instruction *>(E.first)) {
            auto predIdx = getInstructionNumber(instOp);
            auto R2 = LiveRange(predIdx + 1, termIdx);
            instructionInterval_[predIdx].add(R2);
          }
        } // each pred.
      }

    } // for each instruction in the block.
  } // for each block.
}

void VirtualRegisterAllocator::coalesce(
    std::map<Instruction *, Instruction *> &map,
    std::vector<BasicBlock *> &order) {
  // Merge all PHI nodes into a single interval. This part is required for
  // correctness because it bounds the MOV and the PHIs into a single interval.
  for (BasicBlock *BB : order) {
    for (auto &I : *BB) {
      auto *P = dynamic_cast<PhiInst *>(I);
      if (!P)
        continue;

      unsigned phiNum = getInstructionNumber(P);
      for (unsigned i = 0, e = P->getNumEntries(); i < e; ++i) {
        auto *mov = dynamic_cast<MovInst *>(P->getEntry(i).first);

        // Bail out if the interval is already mapped, like in the case of self
        // edges.
        if (map.count(mov))
          continue;

        if (!hasInstructionNumber(mov))
          continue;

        unsigned idx = getInstructionNumber(mov);
        instructionInterval_[phiNum].add(instructionInterval_[idx]);

        // Record the fact that the mov should use the same register as the phi.
        map[mov] = P;
      }
    }
  }
  
  // Given a sequence of MOVs that was generated by the PHI lowering, try to
  // shorten the lifetime of intervals by reusing copies. For example, we
  // shorten the lifetime of %0 by making %2 use %1.
  // %1 = MovInst %0
  // %2 = MovInst %0
  for (BasicBlock *BB : order) {
    std::map<Value *, MovInst *> lastCopy;

    for (auto &I : *BB) {
      auto *mov = dynamic_cast<MovInst *>(I);
      if (!mov)
        continue;
        
      Value *op = mov->getSingleOperand();
      if (dynamic_cast<Literal *>(op))
        continue;

      // If we've made a copy inside this basic block then use the copy.
      auto it = lastCopy.find(op);
      if (it != lastCopy.end()) {
        mov->setOperand(it->second, 0);
      }

      lastCopy[op] = mov;
    }
  }
  
  // Optimize the program by coalescing multiple live intervals into a single
  // long interval. This phase is optional.
  for (BasicBlock *BB : order) {
    for (auto &I : *BB) {
      auto *mov = dynamic_cast<MovInst *>(I);
      if (!mov)
        continue;

      auto *op = dynamic_cast<Instruction *>(mov->getSingleOperand());
      if (!op)
        continue;

      // Don't coalesce intervals that are already coalesced to other intervals
      // or that there are other intervals that are coalesced into it, or if
      // the interval is pre-allocated.
      if (map.count(op) || isAllocated(op) || isAllocated(mov))
        continue;

      // If the MOV is already coalesced into some other interval then merge the
      // operand into that interval.
      Instruction *dest = mov;

      // If the mov is already merged into another interval then find the
      // destination interval and try to merge the current interval into it.
      while (map.count(dest)) {
        dest = map[dest];
      }

      unsigned destIdx = getInstructionNumber(dest);
      unsigned opIdx = getInstructionNumber(op);
      LiveInterval &destIvl = instructionInterval_[destIdx];
      LiveInterval &opIvl = instructionInterval_[opIdx];

      if (destIvl.intersects(opIvl))
        continue;

      for (auto &it : map) {
        if (it.second == op) {
          it.second = dest;
        }
      }

      instructionInterval_[destIdx].add(opIvl);
      map[op] = dest;
    }
  }
}

void VirtualRegisterAllocator::handleInstruction(Instruction *I) {
  // TODO
}

void VirtualRegisterAllocator::allocate() {
  PostOrderAnalysis PO(F);
  std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
  
  resolvePhis(order);
  
  for (auto *BB : order) {
    for (auto &it : *BB) {
      Instruction *I = it;
      auto idx = getInstructionNumber(I);
      (void)idx;
      assert(idx == getInstructionNumber(I) && "Invalid numbering");
    }
  }
  
  F->dump();

  // Init the basic block liveness data structure and calculate the local
  // liveness for each basic block.
  unsigned maxIdx = getMaxInstrIndex();
  for (auto *BB : order) {
    blockLiveness_[BB].init(maxIdx);
  }
  
  for (auto *BB : order) {
    calculateLocalLiveness(blockLiveness_[BB], BB);
  }
  
  // Propagate the local liveness information across the whole function.
  calculateGlobalLiveness(order);

  // Calculate the live intervals for each instruction.
  calculateLiveIntervals(order);

  // Free the memory used for liveness.
  blockLiveness_.clear();
  
  // Maps coalesced instructions. First uses the register allocated for Second.
  std::map<Instruction *, Instruction *> coalesced;
  
  coalesce(coalesced, order);
  
  // Compare two intervals and return the one that starts first.
  auto startsFirst = [&](unsigned a, unsigned b) {
    LiveInterval &IA = instructionInterval_[a];
    LiveInterval &IB = instructionInterval_[b];
    return IA.start() < IB.start() || (IA.start() == IB.start() && a < b);
  };
  
  auto endsFirst = [&](unsigned a, unsigned b) {
    auto &aInterval = instructionInterval_[a];
    auto &bInterval = instructionInterval_[b];
    if (bInterval.end() == aInterval.end()) {
      return bInterval.start() > aInterval.start() ||
          (bInterval.start() == aInterval.start() && b > a);
    }
    return bInterval.end() > aInterval.end();
  };
  
  using InstList = std::vector<unsigned>;
  
  std::priority_queue<unsigned, InstList, decltype(endsFirst)> intervals(
      endsFirst);
  
  for (int i = 0, e = getMaxInstrIndex(); i < e; i++) {
    intervals.push(i);
  }
  
  std::priority_queue<unsigned, InstList, decltype(startsFirst)>
      liveIntervalsQueue(startsFirst);
  
  // Perform the register allocation:
    while (!intervals.empty()) {
      unsigned instIdx = intervals.top();
      intervals.pop();
      Instruction *inst = instructionsByNumbers_[instIdx];
      LiveInterval &instInterval = instructionInterval_[instIdx];
      unsigned currentIndex = instInterval.end();


      // Free all of the intervals that start after the current index.
      while (!liveIntervalsQueue.empty()) {

        unsigned topIdx = liveIntervalsQueue.top();
        LiveInterval &range = instructionInterval_[topIdx];

        // Flush empty intervals and intervals that finished after our index.
        bool nonEmptyInterval = range.size();
        if (range.start() < currentIndex && nonEmptyInterval) {
          break;
        }

        liveIntervalsQueue.pop();

        Instruction *I = instructionsByNumbers_[topIdx];
        VirtualRegister R = getRegister(I);

        registerManager.killRegister(R);

        handleInstruction(I);
      }

      // Don't try to allocate registers that were merged into other live
      // intervals.
      if (coalesced.count(inst)) {
        continue;
      }

      // Allocate a register for the live interval that we are currently handling.
      if (!isAllocated(inst)) {
        VirtualRegister R = registerManager.allocateRegister();
        updateRegister(inst, R);
      }

      // Mark the current instruction as live and remember to perform target
      // specific calls when we are done with the bundle.
      liveIntervalsQueue.push(instIdx);
    } // For each instruction in the function.

    // Free the remaining intervals.
    while (!liveIntervalsQueue.empty()) {
      Instruction *I = instructionsByNumbers_[liveIntervalsQueue.top()];
      registerManager.killRegister(getRegister(I));
      handleInstruction(I);
      liveIntervalsQueue.pop();
    }

    // Allocate registers for the coalesced registers.
    for (auto &RP : coalesced) {
      assert(!isAllocated(RP.first) && "Register should not be allocated");
      Instruction *dest = RP.second;
      updateRegister(RP.first, getRegister(dest));
    }
  
}

VirtualRegister VirtualRegisterAllocator::getRegister(Value *I) {
  assert(isAllocated(I) && "Instruction is not allocated!");
  return allocatedReg[I];
}

void VirtualRegisterAllocator::updateRegister(Value *I, VirtualRegister R) {
  allocatedReg[I] = R;
}

bool VirtualRegisterAllocator::isAllocated(Value *I) {
  return allocatedReg.count(I);
}
