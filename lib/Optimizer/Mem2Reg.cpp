/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define DEBUG_TYPE "mem2reg"

#include "cobra/Optimizer/Mem2Reg.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/CFG.h"

#include <queue>

using namespace cobra;

static bool isAllocaPromotable(AllocStackInst *ASI) {
  for (auto *user : ASI->getUsers()) {
    if (auto *si = dynamic_cast<StoreStackInst *>(user)) {
      if (si->getDest() == ASI)
        return false;
    } else if (auto *si = dynamic_cast<LoadStackInst *>(user)) {
      break;
    } else {
      return false;
    }
  }
  
  return true;
}

static void promoteAllocation(AllocStackInst *ASI) {
  
}

bool Mem2Reg::runOnFunction(Function *F) {
  std::vector<AllocStackInst *> allocas;
  
  F->getParent()->dump();
  
  BasicBlock *BB = F->front();
    
  for (auto *succ : successors(BB)) {
    succ->dump();
  }
  
  
  
  BasicBlock *B = F->back();
  
  B->dump();
  
  auto b = predecessors(B);
  for (auto *pre : predecessors(B)) {
    pre->dump();
  }
  
  bool changed = false;
  
  while (true) {
    allocas.clear();
    
    for (auto I : BB->getInstList()) {
      if (auto *ASI = dynamic_cast<AllocStackInst *>(I))
        if (isAllocaPromotable(ASI))
          allocas.push_back(ASI);
    }
    
    for (auto *ASI : allocas) {
      promoteAllocation(ASI);
    }

    changed = true;
  }
  
  return changed;
}

std::unique_ptr<Pass> cobra::createMem2Reg() {
  return std::make_unique<Mem2Reg>();
}



#undef DEBUG_TYPE
