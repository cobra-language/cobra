/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define DEBUG_TYPE "SSADestruction"

#include "cobra/Optimizer/SSADestruction.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/Analysis.h"

using namespace cobra;

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

bool static lowerPhis(Function *F) {
  bool changed = false;

  std::vector<PhiInst *> PHIs;
  IRBuilder builder(F);
  
  PostOrderAnalysis PO(F);
  std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
  
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
      
      changed = true;
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

  return changed;
}

void static splitCriticalEdge(Function *F) {
  
}

bool static lowerPhis2(Function *F) {
  bool changed = false;

  std::vector<PhiInst *> PHIs;
  IRBuilder builder(F);
  
  PostOrderAnalysis PO(F);
  std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
  
  for (auto BB : order) {
    auto preds = predecessors(BB);
    // Not split critical edges
    if (std::distance(preds.begin(), preds.end()) >= 2)
      continue;
    for (auto Inst : *BB) {
      if (auto *P = dynamic_cast<PhiInst *>(Inst)) {
        PHIs.push_back(P);
      }
    }
  }
  
  for (PhiInst *P : PHIs) {
    for (unsigned i = 0, e = P->getNumEntries(); i < e; ++i) {
      auto E = P->getEntry(i);
      auto *term = E.second->getTerminator();
      builder.setInsertionPoint(term);
      auto *mov = builder.createMovInst(E.first);
      
      
    }
    
  }
  
  
  
  
  
}

bool SSADestruction::runOnFunction(Function *F) {
  bool changed = false;

  changed |= lowerPhis(F);
  
  F->dump();

  return changed;
}

std::unique_ptr<Pass> cobra::createSSADestruction() {
  return std::make_unique<SSADestruction>();
}

#undef DEBUG_TYPE
