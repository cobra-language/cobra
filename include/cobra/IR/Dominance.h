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

class DomTreeNode;
using DomTreeNodePtr = std::shared_ptr<DomTreeNode>;

class DomTreeNode {
public:
  enum class color { WHITE, GRAY, BLACK };

private:
  BasicBlock *TheBB;
  int PostNumber;
  int DFSInNum, DFSOutNum;
  color VisitColor;
  DomTreeNodePtr IDom;

  std::vector<DomTreeNodePtr> Children;
  // Express the predecessor when we depth-first searching of
  // the CFG.
  // e.g.
  //     B1        B2
  //      \        /
  //     \      /
  //      B3
  // B1 and B2 both the father of B3, but we only can via one
  // node reach B3 when the depth-first searching.
  DomTreeNodePtr Father;

  std::vector<DomTreeNodePtr> Predecessors;
public:
  DomTreeNode(BasicBlock *BB = nullptr)
      : TheBB(BB), PostNumber(-1), DFSInNum(-1), DFSOutNum(-1), IDom(nullptr),
        Father(nullptr) {}

  BasicBlock *getBlock() const { return TheBB; }
  DomTreeNodePtr getIDom() const { return IDom; }

  const std::vector<DomTreeNodePtr> &getChildren() const { return Children; }

  DomTreeNodePtr addChild(DomTreeNodePtr Child) {
    Children.push_back(Child);
    return Child;
  }

  unsigned getDFSNumIn() const { return DFSInNum; }
  unsigned getDFSNumOut() const { return DFSOutNum; }
  unsigned getPostOrder() const { return PostNumber; }
  bool DominatedBy(DomTreeNodePtr other) const {
    return this->DFSInNum >= other->DFSInNum &&
           this->DFSOutNum <= other->DFSOutNum;
  }

  void setDFSInNum(int InNum) { DFSInNum = InNum; }
  void setDFSOutNum(int OutNum) { DFSOutNum = OutNum; }
  void setPostNumber(int PostOrder) { PostNumber = PostOrder; }
  void setVisitColor(color c) { VisitColor = c; }
  void setDFSFather(DomTreeNodePtr DFSFather) { Father = DFSFather; }

  color getVisitColor() const { return VisitColor; }
  size_t getNumChildren() const { return Children.size(); }
  void clearAllChildren() { Children.clear(); }

  void setIDom(DomTreeNodePtr NewIDom) { IDom = NewIDom; }
};

class DominatorTree {
  using DomTreeNodeMapType = std::map<BasicBlock *, DomTreeNodePtr>;
  DomTreeNodeMapType DomTreeNodes;
  DomTreeNodePtr RootNode;

  std::vector<DomTreeNodePtr> PostOrder;
  std::vector<DomTreeNodePtr> ReversePostOrder;
  std::list<BasicBlock *> Vertex;

  std::map<DomTreeNodePtr, std::vector<DomTreeNodePtr>> PredecessorrsOfCFG;

  std::map<DomTreeNodePtr, std::set<DomTreeNodePtr>> DominanceFrontier;

  std::vector<DomTreeNodePtr> JoinNodes;

private:
  void getPostOrder();
  void getReversePostOrder();

  void computeDomTree(BasicBlock *EntryBlock);

  std::vector<DomTreeNodePtr> getDomNodePredsFromCFG(DomTreeNodePtr Node);
  // Intersect() - This function only be using to get closest parent of A and B.
  DomTreeNodePtr Intersect(DomTreeNodePtr A, DomTreeNodePtr B);

  void InsertFrontier(DomTreeNodePtr Node, DomTreeNodePtr FrontierItem);

  void ComputeDomFrontier();

public:
  void runOnFunction(Function *F);

  void ComputeDomFrontierOnFunction(Function *F);

  DomTreeNodePtr getDomTreeNode(BasicBlock *BB) const;

  // getRootNode - This returns the entry node for the CFG of the function.
  DomTreeNodePtr getRootNode() { return RootNode; }
  
  bool properlyDominates(DomTreeNodePtr Node) const;
  
  bool isReachableFromEntry(BasicBlock *BB) const;
  
  bool dominates(DomTreeNodePtr A, DomTreeNodePtr B) const;

  void DFS(DomTreeNodePtr Node);

  void Calcuate();

  // dominates - Return true if A dominates B. This perform the special
  // checks necessary if A and B are in the same basic block.
  bool dominates(std::shared_ptr<Instruction> A, std::shared_ptr<Instruction> B) const;
};

}
#endif /* Dominance_h */
