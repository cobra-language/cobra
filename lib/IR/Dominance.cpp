/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Dominance.h"

using namespace cobra;

DominanceInfo::DominanceInfo(Function *F) {
  assert(F->begin() != F->end() && "Function is empty!");
  calculate(F);
}

void DominanceInfo::calculate(Function *F) {
  createReversePostOrder(F);
  createIDoms(F);
  createDominanceFrontier(F);
  createDominanceTreeSuccs(F);
}

void DominanceInfo::postOrderVisit(BasicBlock *BB, std::set<BasicBlock *> &Visited) {
  Visited.insert(BB);
  for (auto B : successors(BB)) {
      if (Visited.find(B) == Visited.end())
        postOrderVisit(B, Visited);
  }
  postOrderID[BB] = reversePostOrder.size();
  reversePostOrder.push_back(BB);
}

BasicBlock *DominanceInfo::intersect(BasicBlock *BB1, BasicBlock *BB2) {
  while (BB1 != BB2) {
    while (postOrderID[BB1] < postOrderID[BB2]) {
      BB1 = getIDom(BB1);
    }
    while (postOrderID[BB2] < postOrderID[BB1]) {
      BB2 = getIDom(BB2);
    }
  }
  return BB1;
}

void DominanceInfo::createReversePostOrder(Function *F) {
  reversePostOrder.clear();
  postOrderID.clear();
  
  std::set<BasicBlock *> visited;
  reversePostOrder.reverse();
}

void DominanceInfo::createIDoms(Function *F) {
  auto Root = F->front();
  setIDom(Root, Root);
  
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto BB : reversePostOrder) {
      if (BB == Root)
        continue;
      
      // (1) Find the first non-nullptr predecessor.
      BasicBlock *pred = nullptr;
      for (auto predecessor : predecessors(BB)) {
        if (getIDom(predecessor)) {
          pred = predecessor;
          break;
        }
      }
      
      // (2) Traverse other predecessors.
      BasicBlock *newIDom = pred;
      for (auto *predecessor : predecessors(BB)) {
        if (predecessor == pred)
          continue;
        
        if (getIDom(predecessor))
          newIDom = intersect(predecessor, newIDom);
      }
      
      // (3) Judge the IDom is changed.
      if (getIDom(BB) != newIDom) {
        setIDom(BB, newIDom);
        changed = true;
      }
    }
  }
}

void DominanceInfo::createDominanceFrontier(Function *F) {
  for (auto &BB : F->getBasicBlockList()) {
    auto preds = predecessors(BB);
    if (std::distance(preds.begin(), preds.end()) >= 2) {
      for (auto *pred : preds) {
        auto runner = pred;
        while (runner != getIDom(BB)) {
          addDominanceFrontier(runner, BB);
          runner = getIDom(runner);
        }
      }
    }
  }
}

void DominanceInfo::createDominanceTreeSuccs(Function *F) {
  for (auto &BB : F->getBasicBlockList()) {
    auto idom = getIDom(BB);
    if (idom != BB)
      addDominanceTreeSucc(idom, BB);
  }
}
