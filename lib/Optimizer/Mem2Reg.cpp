/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define DEBUG_TYPE "mem2reg"

#include <queue>
#include <unordered_set>

#include "cobra/Optimizer/Mem2Reg.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/CFG.h"
#include "cobra/IR/Dominance.h"

using namespace cobra;

using NodePriorityQueue = std::priority_queue<BasicBlock *>;
using BlockToInstMap = std::map<BasicBlock *, Instruction *>;

static bool isAllocaPromotable(AllocStackInst *ASI) {
  for (auto *user : ASI->getUsers()) {
    if (auto *si = dynamic_cast<StoreStackInst *>(user)) {
      if (si->getSrc() == ASI)
        return false;
    } else if (auto *si = dynamic_cast<LoadStackInst *>(user)) {
      break;
    } else {
      return false;
    }
  }
  
  return true;
}

static void collectStackAllocations(Function *F, std::vector<AllocStackInst *> &allocas) {
  BasicBlock *BB = F->front();
  for (auto I : BB->getInstList()) {
    if (auto *ASI = dynamic_cast<AllocStackInst *>(I))
      if (isAllocaPromotable(ASI))
        allocas.push_back(ASI);
  }
}

static void promoteAllocStackToSSA(AllocStackInst *ASI, DominanceInfo &DT) {
  NodePriorityQueue PQ;
  std::unordered_set<BasicBlock *> phiBlocks;
  
  ASI->dump();
  
  for (auto *U : ASI->getUsers()) {
    if (dynamic_cast<StoreStackInst *>(U)) {
      PQ.push(U->getParent());
    }
  }
  
  std::unordered_set<BasicBlock *> visited;
  std::vector<BasicBlock *> workList;
  
  while (!PQ.empty()) {
    BasicBlock *root = PQ.top();
    PQ.pop();
    
    workList.clear();
    workList.push_back(root);
    
    while (!workList.empty()) {
      auto BB = workList.back();
      workList.pop_back();
      BB->dump();
      
      for (auto frontier : DT.getDominanceFrontier(BB)) {
        if (visited.insert(frontier).second) {
          continue;
        }
        
        if (phiBlocks.insert(frontier).second) {
          PQ.push(frontier);
          workList.push_back(frontier);
        }
      }
    }
  }
  
  BlockToInstMap phiLoc;
  
  IRBuilder builder(ASI->getParent()->getParent());
    
  for (auto *BB : phiBlocks) {
    builder.setInsertionPoint(*BB->begin());
    // Create an empty phi node and register it as the phi for ASI for block BB.
    phiLoc[BB] = builder.createPhiInst();
    
    BB->dump();
  }
}

bool Mem2Reg::runOnFunction(Function *F) {
  F->dump();
  
  bool changed = false;
  DominanceInfo DT(F);
  
  auto B = F->front();
  B->dump();
  
  for (auto *succ : successors(B)) {
    succ->dump();
    for (auto frontier : DT.getDominanceFrontier(succ)) {
      frontier->dump();
    }
  }
  
  std::vector<AllocStackInst *> allocas;
  
  collectStackAllocations(F, allocas);
  
  for (auto *ASI : allocas) {
    promoteAllocStackToSSA(ASI, DT);
  }
  
  return changed;
}

std::unique_ptr<Pass> cobra::createMem2Reg() {
  return std::make_unique<Mem2Reg>();
}



#undef DEBUG_TYPE
