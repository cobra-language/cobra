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

using DomTreeNodePair = std::pair<DomTreeNode *, unsigned>;
using NodePriorityQueue = std::priority_queue<DomTreeNodePair>;
using BlockToInstMap = std::map<BasicBlock *, Instruction *>;
using DomTreeLevelMap = std::map<DomTreeNode *, unsigned>;

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

static void computeDomTreeLevels(DominanceInfo *DT, DomTreeLevelMap &DomTreeLevels) {
  std::vector<DomTreeNode *> worklist;
  DomTreeNode *root = DT->getRootNode();

  DomTreeLevels[root] = 0;
  worklist.push_back(root);

  while (!worklist.empty()) {
    DomTreeNode *Node = worklist.back();
    worklist.pop_back();
    
    unsigned ChildLevel = DomTreeLevels[Node] + 1;
    for (auto &CI : *Node) {
      DomTreeLevels[CI] = ChildLevel;
      worklist.push_back(CI);
    }
  }
}

static Value *getLiveOutValue(
    BasicBlock *startBB,
    BlockToInstMap &phiLoc,
    DominanceInfo &DT,
    BlockToInstMap &stores) {
  // Walk the Dom tree in search of a defining value:
  for (DomTreeNode *Node = DT.getNode(startBB); Node; Node = Node->getIDom()) {
    BasicBlock *BB = Node->getBlock();

    // If there is a store (that must come after the phi), use its value.
    auto it = stores.find(BB);
    if (it != stores.end()) {
      return dynamic_cast<StoreStackInst *>(it->second)->getSrc();
    }

    // If there is a Phi definition in this block:
    auto pit = phiLoc.find(BB);
    if (pit != phiLoc.end()) {
      return pit->second;
    }
    // Move to the next dominating block.
  }
  IRBuilder builder(startBB->getParent());
  return builder.getLiteralUndefined();
}

/// \returns the live-in value for basic block \p BB knowing that we've placed
/// phi nodes in the blocks \p phiLoc.
static Value *getLiveInValue(
    BasicBlock *BB,
    BlockToInstMap &phiLoc,
    DominanceInfo &DT,
    BlockToInstMap &stores) {
  // If we are looking for a value in the current basic block then it must be
  // the phi node of the current block.
  auto it = phiLoc.find(BB);
  if (it != phiLoc.end())
    return it->second;

  auto *node = DT.getNode(BB);
  if (!node) {
    // If the node is not in the dom tree then it means that the current value
    // is unreachable. Just return undef.
    IRBuilder builder(BB->getParent());
    return builder.getLiteralUndefined();
  }

  assert(DT.getNode(BB) && "Block must be a part of the dom tree");

  // If the value is not defined in the current basic block then it means that
  // it is defined in one of the dominating basic blocks that are flowing into
  // the current basic block.

  DomTreeNode *IDom = node->getIDom();
  assert(IDom && "Reached the top of the tree!");

  return getLiveOutValue(IDom->getBlock(), phiLoc, DT, stores);
}

static void promoteAllocationToPhi(
    AllocStackInst *ASI,
    DominanceInfo &DT,
    DomTreeLevelMap &domTreeLevels) {
  // A list of blocks that will require new Phi values.
  std::unordered_set<BasicBlock *> phiBlocks;
  
  // The "piggy-bank" data-structure that we use for processing the dom-tree
  // bottom-up.
  NodePriorityQueue priorityQueue;

  ASI->dump();

  // Collect all of the stores into the AllocStack. We know that at this point
  // we have at most one store per block.
  for (auto *user : ASI->getUsers()) {
    // We need to place Phis for this block.
    if (dynamic_cast<StoreStackInst *>(user)) {
      // If the block is in the dom tree (dominated by the entry block).
      if (DomTreeNode *node = DT.getNode(user->getParent())) {
        priorityQueue.push(std::make_pair(node, domTreeLevels[node]));
      }
    }
  }

  // A list of nodes for which we already calculated the dominator frontier.
  std::unordered_set<DomTreeNode *> visited;
  std::vector<DomTreeNode *> worklist;

  // Scan all of the definitions in the function bottom-up using the priority
  // queue.
  while (!priorityQueue.empty()) {
    DomTreeNodePair rootPair = priorityQueue.top();
    priorityQueue.pop();
    DomTreeNode *root = rootPair.first;
    unsigned rootLevel = rootPair.second;

    // Walk all dom tree children of Root, inspecting their successors. Only
    // J-edges, whose target level is at most Root's level are added to the
    // dominance frontier.
    worklist.clear();
    worklist.push_back(root);

    while (!worklist.empty()) {
      auto node = worklist.back();
      worklist.pop_back();
      BasicBlock *BB = node->getBlock();
      
      for (auto succ : successors(BB)) {
        DomTreeNode *succNode = DT.getNode(succ);
        
        // Skip D-edges (edges that are dom-tree edges).
        if (succNode->getIDom() == node)
          continue;
        
        // Ignore J-edges that point to nodes that are not smaller or equal
        // to the root level.
        unsigned succLevel = domTreeLevels[succNode];
        if (succLevel > rootLevel)
          continue;
        
        // Ignore visited nodes.
        if (!visited.insert(succNode).second)
          continue;
        
        // If the new PHInode is properly dominated by the deallocation then it
        // is obviously a dead PHInode, so we don't need to insert it.
        if (!DT.dominates(ASI->getParent(), succNode->getBlock()))
          continue;
        
        // The successor node is a new PHINode. If this is a new PHI node
        // then it may require additional definitions, so add it to the PQ.
        if (phiBlocks.insert(succ).second)
          priorityQueue.push(std::make_pair(succNode, succLevel));
      }
      
      // Add the children in the dom-tree to the worklist.
      for (auto &CI : *node)
        if (!visited.count(CI))
          worklist.push_back(CI);
    }
  }
  
  // At this point we calculated the locations of all of the new Phi values.
  // Next, add the Phi values and promote all of the loads and stores into the
  // new locations.

  BlockToInstMap phiLoc;

  IRBuilder builder(ASI->getParent()->getParent());

  for (auto *BB : phiBlocks) {
    builder.setInsertionPoint(*BB->begin());
    // Create an empty phi node and register it as the phi for ASI for block BB.
    phiLoc[BB] = builder.createPhiInst();
  }
  
  BlockToInstMap stores;
  std::vector<LoadStackInst *> loads;
  
  // Collect all loads/stores for the ASI.
  for (auto *U : ASI->getUsers()) {
    if (auto *L = dynamic_cast<LoadStackInst *>(U)) {
      loads.push_back(L);
      continue;
    }
    if (auto *S = dynamic_cast<StoreStackInst *>(U)) {
      assert(!stores.count(S->getParent()) && "multiple stores per block!");
      stores[S->getParent()] = S;
      continue;
    }
  }
  
  // For all phi nodes we've decided to insert:
  for (auto *BB : phiBlocks) {
    auto *phi = dynamic_cast<PhiInst *>(phiLoc[BB]);

    std::unordered_set<BasicBlock *> processed{};
    for (auto *pred : predecessors(BB)) {
      // The predecessor list can contain duplicates. Just skip them.
      if (!processed.insert(pred).second)
        continue;

      auto *val = getLiveOutValue(pred, phiLoc, DT, stores);
      phi->addEntry(val, pred);
    }
  }
  
  {
    IRBuilder::InstructionDestroyer destroyer;

    // Replace all loads with the new phi nodes.
    for (auto &ld : loads) {
      auto *repl = getLiveInValue(ld->getParent(), phiLoc, DT, stores);
      ld->replaceAllUsesWith(repl);
      destroyer.add(ld);
    }

    // Delete all stores.
    for (auto &st : stores) {
      destroyer.add(st.second);
    }
  }
}

static void pruneAllocStackUsage(Function *F) {
  
}

bool Mem2Reg::runOnFunction(Function *F) {
  bool changed = false;
  DominanceInfo DT(F);
  
  DomTreeLevelMap domTreeLevels;
  computeDomTreeLevels(&DT, domTreeLevels);
  
  // a list of stack allocations to promote.
  std::vector<AllocStackInst *> allocations;
  
  collectStackAllocations(F, allocations);
  
  for (auto *ASI : allocations) {
    promoteAllocationToPhi(ASI, DT, domTreeLevels);
  }
  
  return changed;
}

std::unique_ptr<Pass> cobra::createMem2Reg() {
  return std::make_unique<Mem2Reg>();
}



#undef DEBUG_TYPE
