/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Dominance.h"

using namespace cobra;
using color = DomTreeNode::color;

DomTreeNodePtr DominatorTree::getDomTreeNode(BasicBlock *BB) const {
  auto Result = DomTreeNodes.find(BB);
  if (Result != DomTreeNodes.end())
    return Result->second;
  assert(0 && "Unreachable code.");
  return nullptr;
}

void DominatorTree::runOnFunction(Function *F) {
  Vertex = F->getBasicBlockList();
  computeDomTree(F->front());
}

// compute the DomTree.
void DominatorTree::computeDomTree(BasicBlock *EntryBlock) {
  for (const auto &item : Vertex) {
    DomTreeNodes.insert({item, std::make_shared<DomTreeNode>(item)});
  }

  auto FindEntry = getDomTreeNode(EntryBlock);
  assert(FindEntry && "must have entry node");
  RootNode = FindEntry;

  for (auto item : DomTreeNodes)
    item.second->setVisitColor(color::WHITE);
  dfs(RootNode);

  calcuate();
}

std::set<DomTreeNodePtr> &DominatorTree::getDominanceFrontier(BasicBlock *BB) {
  auto Node = getDomTreeNode(BB);
  return DominanceFrontier[Node];
}

void DominatorTree::dfs(DomTreeNodePtr Node) {
  static int DFSCounter = 0;
  static int PostOrder = 0;
  Node->setDFSInNum(DFSCounter++);
  Node->setVisitColor(color::GRAY);

  auto TermiInst = Node->getBlock()->getTerminator();
  if (TermiInst == nullptr) {
    Node->setDFSOutNum(DFSCounter++);
    Node->setPostNumber(PostOrder++);
    Node->setVisitColor(color::BLACK);
    return;
  }

  std::vector<DomTreeNodePtr> SuccNodes;
  for (unsigned i = 0, size = TermiInst->getNumSuccessors(); i < size; i++) {
    SuccNodes.push_back(getDomTreeNode(TermiInst->getSuccessor(i)));
  }

  for (const auto &item : SuccNodes) {
    if (item->getVisitColor() == color::WHITE) {
      item->setDFSFather(Node);
      dfs(item);
    }
  }
  Node->setDFSOutNum(DFSCounter++);
  Node->setPostNumber(PostOrder++);
  Node->setVisitColor(color::BLACK);
}

void DominatorTree::getPostOrder() {
  for (const auto &item : DomTreeNodes)
    item.second->setVisitColor(color::WHITE);

  for (std::size_t i = 0, size = DomTreeNodes.size(); i < size; ++i) {
    size_t flag = DomTreeNodes.size();
    DomTreeNodePtr tmp = nullptr;

    for (const auto &item : DomTreeNodes) {
      if (item.second->getPostOrder() < flag &&
          item.second->getVisitColor() != color::BLACK) {
        flag = item.second->getPostOrder();
        tmp = item.second;
      }
    }

    tmp->setVisitColor(color::BLACK);
    PostOrder.push_back(tmp);
  }
}

void DominatorTree::getReversePostOrder() {
  if (PostOrder.size() == 0)
    getPostOrder();
  for (int i = PostOrder.size() - 1; i >= 0; --i)
    ReversePostOrder.push_back(PostOrder[i]);
}

std::vector<DomTreeNodePtr>
DominatorTree::getDomNodePredsFromCFG(DomTreeNodePtr Node) {
  auto Result = PredecessorrsOfCFG.find(Node);
  if (Result != PredecessorrsOfCFG.end())
    return Result->second;

  std::vector<DomTreeNodePtr> PredDomTreeNode;
  auto BB = Node->getBlock();
  auto Preds = predecessors(BB);
  int PredCount = 0;

  for (const auto &pred : Preds) {
    PredDomTreeNode.push_back(getDomTreeNode(pred));
    PredCount += 1;
  }

  if (PredCount > 1)
    JoinNodes.push_back(Node);
  PredecessorrsOfCFG.insert({Node, PredDomTreeNode});
  return PredDomTreeNode;
}

DomTreeNodePtr DominatorTree::intersect(DomTreeNodePtr A, DomTreeNodePtr B) {
  DomTreeNodePtr finger1 = A;
  DomTreeNodePtr finger2 = B;

  while (finger1 != finger2) {
    while (finger1->getPostOrder() < finger2->getPostOrder())
      finger1 = finger1->getIDom();
    while (finger2->getPostOrder() < finger1->getPostOrder())
      finger2 = finger2->getIDom();
  }
  return finger1;
}

void DominatorTree::calcuate() {
  if (ReversePostOrder.size() == 0)
    getReversePostOrder();

  bool changed = true;
  RootNode->setIDom(RootNode);

  while (changed) {
    changed = false;
    for (const auto &CurNode : ReversePostOrder) {
      if (CurNode == RootNode)
        continue;

      auto PredDomNodeFromCFG = getDomNodePredsFromCFG(CurNode);

      auto getAvailiablePred = [&PredDomNodeFromCFG]() -> DomTreeNodePtr {
        for (auto pred : PredDomNodeFromCFG) {
          if (pred->getIDom() != nullptr)
            return pred;
        }
        assert(0 && "Unreachable code.");
        return nullptr;
      };

      auto AvailiablePred = getAvailiablePred();
      DomTreeNodePtr NewIDom = AvailiablePred;

      // (2) Traverse other predecessors.
      for (const auto &pred : PredDomNodeFromCFG) {
        if (pred == NewIDom)
          continue;
        if (pred->getIDom() != nullptr)
          NewIDom = intersect(NewIDom, pred);
      }

      // (3) Judge the IDom is changed.
      if (CurNode->getIDom() != NewIDom) {
        CurNode->setIDom(NewIDom);
        changed = false;
      }
    }
  }
}

void DominatorTree::insertFrontier(DomTreeNodePtr Node,
                                   DomTreeNodePtr FrontierItem) {
  auto NodeEntry = DominanceFrontier.find(Node);
  if (NodeEntry != DominanceFrontier.end()) {
    NodeEntry->second.insert(FrontierItem);
  } else {
    std::set<DomTreeNodePtr> Frontier;
    Frontier.insert(FrontierItem);
    DominanceFrontier.insert({Node, Frontier});
  }
}

void DominatorTree::computeDomFrontier() {
  DomTreeNodePtr runner = nullptr;
  // Just compute the join points.
  for (const auto &Node : JoinNodes) {
    auto preds = getDomNodePredsFromCFG(Node);
    for (auto pred : preds) {
      runner = pred;
      while (runner != Node->getIDom()) {
        insertFrontier(runner, Node);
        runner = runner->getIDom();
      }
    }
  }
}

void DominatorTree::computeDomFrontierOnFunction(Function *F) {
  if (RootNode->getIDom() == nullptr)
    runOnFunction(F);
  computeDomFrontier();
}
