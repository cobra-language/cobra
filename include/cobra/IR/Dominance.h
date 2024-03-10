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
#include <unordered_set>

#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/CFG.h"

namespace cobra {

class DomTreeNode {
  BasicBlock *TheBB;
  DomTreeNode *IDom;
  unsigned Level;
  
  std::vector<DomTreeNode *> Children;
  mutable unsigned DFSNumIn = ~0;
  mutable unsigned DFSNumOut = ~0;
  
public:
  DomTreeNode(BasicBlock *BB, DomTreeNode *iDom)
      : TheBB(BB), IDom(iDom), Level(IDom ? IDom->Level + 1 : 0) {}
  
  using iterator = typename std::vector<DomTreeNode *>::iterator;
  
  iterator begin() { return Children.begin(); }
  iterator end() { return Children.end(); }
  
  BasicBlock *getBlock() const { return TheBB; }
  DomTreeNode *getIDom() const { return IDom; }
  unsigned getLevel() const { return Level; }
  
  const std::vector<DomTreeNode *> &getChildren() const { return Children; }
  
  std::unique_ptr<DomTreeNode> addChild(
      std::unique_ptr<DomTreeNode> C) {
    Children.push_back(C.get());
    return C;
  }
  
  bool isLeaf() const { return Children.empty(); }
  size_t getNumChildren() const { return Children.size(); }
  
  void clearAllChildren() { Children.clear(); }
  
  void setIDom(DomTreeNode *NewIDom) {
    if (!IDom) {
      IDom = NewIDom;
      Level = NewIDom->Level + 1;
    }
    
    if (IDom == NewIDom) return;
    
    auto I = std::find(IDom->Children.begin(), IDom->Children.end(), this);
    assert(I != IDom->Children.end() &&
           "Not in immediate dominator children set!");
    // I am no longer your child...
    IDom->Children.erase(I);

    // Switch to new dominator
    IDom = NewIDom;
    IDom->Children.push_back(this);

    UpdateLevel();
  }
  
  unsigned getDFSNumIn() const { return DFSNumIn; }
  unsigned getDFSNumOut() const { return DFSNumOut; }
  
private:
  
  void UpdateLevel() {
    assert(IDom);
    if (Level == IDom->Level + 1) return;

    std::vector<DomTreeNode*> WorkStack = {this};

    while (!WorkStack.empty()) {
      DomTreeNode *Current = WorkStack.back();
      WorkStack.pop_back();
      Current->Level = Current->IDom->Level + 1;

      for (DomTreeNode *C : Current->getChildren()) {
        assert(C->IDom);
        if (C->Level != C->IDom->Level + 1) WorkStack.push_back(C);
      }
    }
  }
};

class DominanceInfo {
  Function *Parent;
  
  using DomTreeNodeMapType =
     std::map<BasicBlock *, std::unique_ptr<DomTreeNode>>;
  DomTreeNodeMapType DomTreeNodes;
  DomTreeNode *RootNode;
  
  std::list<DomTreeNode *> ReversePostOrder;
  std::map<DomTreeNode *, int> PostOrderMap;
  
  std::map<DomTreeNode *, std::vector<DomTreeNode *>> PredecessorrsOfNode;
  
  std::vector<DomTreeNode *> JoinNodes;
  
  std::map<DomTreeNode *, std::set<DomTreeNode *>> DominanceFrontier;
  std::map<DomTreeNode *, std::set<DomTreeNode *>> DominanceTreeSuccs;
  
private:
  void calculate();
  void createReversePostOrder();
  void createIDoms();
  void createDominanceFrontier();
  void createDominanceTreeSuccs();
  
  void postOrderVisit(BasicBlock *BB, std::set<BasicBlock *> &Visited);
  
  // Intersect() - This function only be using to get closest parent of A and B.
  DomTreeNode *intersect(DomTreeNode *A, DomTreeNode *B);
  
  std::vector<DomTreeNode *> getDomNodePreds(DomTreeNode *Node);

public:
  explicit DominanceInfo(Function *F);
  
  DomTreeNode *getRootNode() { return RootNode; }
  
  DomTreeNode *getNode(BasicBlock *BB) const {
    auto I = DomTreeNodes.find(BB);
    if (I != DomTreeNodes.end())
      return I->second.get();
    return nullptr;
  }
  
  DomTreeNode *operator[](BasicBlock *BB) const {
    return getNode(BB);
  }
  
  void addDominanceFrontier(DomTreeNode *A, DomTreeNode *B) {
    DominanceFrontier[A].insert(B);
  }
  
  std::set<DomTreeNode *> &getDominanceFrontier(DomTreeNode *BB) {
    return DominanceFrontier[BB];
  }
  
  std::set<DomTreeNode *> getDominanceTreeSucc(DomTreeNode *BB) {
    return DominanceTreeSuccs[BB];
  }
  
  void addDominanceTreeSucc(DomTreeNode *A, DomTreeNode *B) {
    DominanceTreeSuccs[A].insert(B);
  }
  
  /// properlyDominates - Returns true iff A dominates B and A != B.
  /// Note that this is not a constant time operation!
  bool properlyDominates(DomTreeNode *A, DomTreeNode *B) const {
    if (!A || !B)
      return false;
    if (A == B)
      return false;
    return dominates(A, B);
  }
  
  bool properlyDominates(BasicBlock *A, BasicBlock *B) const {
    return properlyDominates(getNode(A), getNode(B));
  }
  
  /// dominates - Returns true iff A dominates B.  Note that this is not a
  /// constant time operation!
  bool dominates(DomTreeNode *A, DomTreeNode *B) const {
    if (A == B)
      return true;

    // The EntryBlock dominates every other block.
    if (A == RootNode)
      return true;

    // Note: The dominator of the EntryBlock is itself.
    DomTreeNode *IDom = B->getIDom();
    while (IDom != A && IDom != RootNode)
      IDom = IDom->getIDom();

    return IDom != RootNode;
  }
  
  bool dominates(BasicBlock *A, BasicBlock *B) const {
    if (A == B)
      return true;
    
    return dominates(getNode(A), getNode(B));
  }
  
  bool properlyDominates(Instruction *A, Instruction *B) const {
    BasicBlock *ABB = A->getParent();
    BasicBlock *BBB = B->getParent();
    
    if (ABB != BBB)
      return properlyDominates(ABB, BBB);
    
    // Otherwise, they're in the same block, and we just need to check
    // whether B comes after A.
    auto ItA = ABB->getIterator(A);
    auto ItB = BBB->getIterator(B);
    auto E = ABB->begin();
    while (ItB != E) {
      --ItB;
      if (ItA == ItB)
        return true;
    }
    return false;
  }
  
};

}
#endif /* Dominance_h */
