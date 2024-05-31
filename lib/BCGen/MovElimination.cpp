/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/MovElimination.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/Analysis.h"
#include "cobra/BCGen/RegAlloc.h"

using namespace cobra;

bool MovElimination::runOnFunction(Function *F) {
  bool changed = false;

  // Keeps track of last assignment point of each register.
  std::unordered_map<VirtualRegister, unsigned, VirtualRegister::Hash> lastAssignment;
  // Keeps track of last use point of each register.
  std::unordered_map<VirtualRegister, unsigned, VirtualRegister::Hash> lastUse;

  IRBuilder::InstructionDestroyer destroyer;

  // For each basic block, do a forward scan and remember when each variable was
  // last assigned. Use this information to remove MOVs.
  PostOrderAnalysis PO(F);
  std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
  for (auto *BB : order) {
    unsigned index = 0;
    lastAssignment.clear();
    lastUse.clear();
    for (auto &it : *BB) {
      // Skip basic blocks with unallocated instructions.
      if (!RA_.isAllocated(it))
        continue;

      // Set to true if the current instruction is a mov which we eliminated.
      bool movRemoved = false;

      index++;
      VirtualRegister dest = RA_.getRegister(it);

      if (auto *mov = dynamic_cast<MovInst *>(it)) {
        Value *op = mov->getSingleOperand();
        // If the operand is an instruction in the current basic block and it
        // has one user then maybe we can write it directly into the target
        // register.
        auto *IOp = dynamic_cast<Instruction *>(op);

        // Skip basic blocks with unallocated instructions.
        if (!RA_.isAllocated(op))
          continue;

        if (IOp && op->hasOneUser() && IOp->getParent() == BB) {
          VirtualRegister src = RA_.getRegister(IOp);
          // Get the index of the instructions that last wrote to the source and
          // dest registers. Note lookup() returns 0 if not found.
          auto iter = lastAssignment.find(src);
          unsigned srcIdx = iter != lastAssignment.end() ? iter->second : 0;
          iter = lastAssignment.find(dest);
          unsigned destIdx = iter != lastAssignment.end() ? iter->second : 0;
          iter = lastUse.find(dest);
          unsigned destUseIdx = iter != lastUse.end() ? iter->second : 0;

          // If the dest register was last written *after* the src register was
          // written into then we know that it is *live* in the range src..dest
          // so we can't remove the MOV. Only if the dest was live before the
          // src can we remove it.
          // Additionally, dest must not have uses in the range (src..dest).
          if (destIdx < srcIdx && !dynamic_cast<PhiInst *>(IOp) &&
              destUseIdx <= srcIdx) {
            RA_.updateRegister(op, dest);
            destroyer.add(mov);
            mov->replaceAllUsesWith(op);
            changed = true;
            movRemoved = true;
          }
        }
      }

      // Save the current instruction and report the last index where the
      // register was modified.
      lastAssignment[dest] = index;

      // Save the last use point of every register, but skip mov-s which we just
      // eliminated.
      if (!movRemoved) {
        for (unsigned i = 0, e = it->getNumOperands(); i != e; ++i) {
          auto *op = it->getOperand(i);
          if (RA_.isAllocated(op))
            lastUse[RA_.getRegister(op)] = index;
        }
      }
    }
  }

  return changed;
}

