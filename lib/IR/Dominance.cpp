/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Dominance.h"

using namespace cobra;

DominanceInfo::DominanceInfo(Function *F) : Parent(F) {
  assert(F->begin() != F->end() && "Function is empty!");
  calculate();
}

void DominanceInfo::calculate() {
  for (auto BB : *Parent)
    DomTreeNodes.insert({BB, std::make_unique<DomTreeNode>(BB, nullptr)});
  
  auto FindEntry = getNode(Parent->front());
  assert(FindEntry && "must have entry node");
  RootNode = FindEntry;
  
  createReversePostOrder();
  createIDoms();
  createDominanceFrontier();
  createDominanceTreeSuccs();
}

void DominanceInfo::postOrderVisit(BasicBlock *BB, std::set<BasicBlock *> &Visited) {
  Visited.insert(BB);
  for (auto B : successors(BB)) {
      if (Visited.find(B) == Visited.end())
        postOrderVisit(B, Visited);
  }
  PostOrderMap[getNode(BB)] = ReversePostOrder.size();
  ReversePostOrder.push_back(getNode(BB));
}

DomTreeNode *DominanceInfo::intersect(DomTreeNode *A, DomTreeNode *B) {
  while (A != B) {
    while (PostOrderMap[A] < PostOrderMap[B]) {
      A = A->getIDom();
    }
    while (PostOrderMap[B] < PostOrderMap[A]) {
      B = B->getIDom();
    }
  }
  return A;
}

std::vector<DomTreeNode *> DominanceInfo::getDomNodePreds(DomTreeNode *Node) {
  auto R = PredecessorrsOfNode.find(Node);
  if (R != PredecessorrsOfNode.end())
    return R->second;
  
  std::vector<DomTreeNode *> PredDomTreeNodes;
  auto preds = predecessors(Node->getBlock());
  
  for (auto pre : preds)
    PredDomTreeNodes.push_back(getNode(pre));
  
  if (std::distance(preds.begin(), preds.end()) >= 2)
    JoinNodes.push_back(Node);
  
  PredecessorrsOfNode.insert({Node, PredDomTreeNodes});
  return PredDomTreeNodes;
}

void DominanceInfo::createReversePostOrder() {
  ReversePostOrder.clear();
  PostOrderMap.clear();
  
  std::set<BasicBlock *> visited;
  postOrderVisit(Parent->front(), visited);
  ReversePostOrder.reverse();
}

void DominanceInfo::createIDoms() {
  RootNode->setIDom(RootNode);
  
  bool changed = true;
  while (changed) {
    changed = false;
    for (auto Node : ReversePostOrder) {
      if (Node == RootNode)
        continue;
      
      // (1) Find the first non-nullptr predecessor.
      DomTreeNode *IDom = nullptr;
      for (auto pred : getDomNodePreds(Node)) {
        if (pred->getIDom()) {
          IDom = pred;
          break;
        }
      }
      
      // (2) Traverse other predecessors.
      DomTreeNode *newIDom = IDom;
      for (auto *pred : getDomNodePreds(Node)) {
        if (pred == IDom)
          continue;
        
        if (pred->getIDom())
          newIDom = intersect(pred, newIDom);
      }
      
      // (3) Judge the IDom is changed.
      if (Node->getIDom() != newIDom) {
        Node->setIDom(newIDom);
        changed = true;
      }
    }
  }
}

void DominanceInfo::createDominanceFrontier() {
  for (auto &Node : JoinNodes) {
    for (auto *pred : getDomNodePreds(Node)) {
      auto runner = pred;
      while (runner != Node->getIDom()) {
        addDominanceFrontier(runner, Node);
        runner = runner->getIDom();
      }
    }
  }
}

void DominanceInfo::createDominanceTreeSuccs() {
  for (auto &Node : JoinNodes) {
    auto idom = Node->getIDom();
    if (idom != Node)
      addDominanceTreeSucc(idom, Node);
  }
}
