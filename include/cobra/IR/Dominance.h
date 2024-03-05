/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

//===------------------------------Dominance.h---------------------------===//
//
// This file defines the following classes:
//  1. DominatorTree: Represent dominators as an explicit tree structure.
//  2. DominatorFrontier: Calculate and hold the dominance frontier for a
//    function.
// Use the "A Simple, Fast Dominance Algorithm.Keith D.Cooper" to construct
// dominator tree.
//
//===---------------------------------------------------------------------===//

#ifndef Dominance_h
#define Dominance_h

#include <stdio.h>
#include <map>
#include <set>
#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/CFG.h"

namespace cobra {

class DominanceInfo {
private:  
  std::list<BasicBlock *> reversePostOrder;
  std::map<BasicBlock *, int> postOrderID;
  
  std::map<BasicBlock *, std::set<BasicBlock *>> Doms; // dominators set
  std::map<BasicBlock *, BasicBlock *> IDoms;  // immediate dominators
  std::map<BasicBlock *, std::set<BasicBlock *>> dominanceFrontier;
  std::map<BasicBlock *, std::set<BasicBlock *>> dominanceTreeSuccs;
  
  void calculate(Function *F);
  void postOrderVisit(BasicBlock *BB, std::set<BasicBlock *> &Visited);
  
  BasicBlock *intersect(BasicBlock *BB1, BasicBlock *BB2);
  
public:
  explicit DominanceInfo(Function *F);
  
  void createReversePostOrder(Function *F);
  void createIDoms(Function *F);
  void createDominanceFrontier(Function *F);
  void createDominanceTreeSuccs(Function *F);
  
  void addDom(BasicBlock *BB, BasicBlock *domBB) { Doms[BB].insert(domBB); }
  std::set<BasicBlock *> &getDoms(BasicBlock *BB) { return Doms[BB]; }
  void setDoms(BasicBlock *BB, std::set<BasicBlock *> &newDoms) {
    Doms[BB].clear();
    Doms[BB].insert(newDoms.begin(), newDoms.end());
  }

  BasicBlock *getIDom(BasicBlock *BB) { return IDoms[BB]; }
  void setIDom(BasicBlock *BB, BasicBlock *IDom) { IDoms[BB] = IDom; }
  
  void addDominanceFrontier(BasicBlock *BB1, BasicBlock *BB2) {
    dominanceFrontier[BB1].insert(BB2);
  }
  std::set<BasicBlock *> &getDominanceFrontier(BasicBlock *BB) {
    return dominanceFrontier[BB];
  }
  
  std::set<BasicBlock *> getDominanceTreeSucc(BasicBlock *BB) {
    return dominanceTreeSuccs[BB];
  }
  void addDominanceTreeSucc(BasicBlock *BB1, BasicBlock *BB2) {
    dominanceTreeSuccs[BB1].insert(BB2);
  }
  
};

}
#endif /* Dominance_h */
