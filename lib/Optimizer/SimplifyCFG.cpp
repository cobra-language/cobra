/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define DEBUG_TYPE "simplifycfg"

#include <unordered_set>

#include "cobra/Optimizer/SimplifyCFG.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/CFG.h"

using namespace cobra;

static int NumUnreachableBlock = 0;

static void deleteBasicBlock(BasicBlock *B) {
  Value::UseListTy users(B->getUsers().begin(), B->getUsers().end());
  
  for (auto *I : users) {
    if (auto *Phi = dynamic_cast<PhiInst *>(I)) {
      Phi->removeEntry(B);
      continue;
    }
  }
  
  B->replaceAllUsesWith(nullptr);
  B->eraseFromParent();
}

static bool removeUnreachedBasicBlocks(Function *F) {
  bool changed = false;
  
  std::unordered_set<BasicBlock *> visited;
  std::vector<BasicBlock *> workList;
  
  workList.push_back(*F->begin());
  while (!workList.empty()) {
    auto BB = workList.back();
    workList.pop_back();
        
    if (!visited.insert(BB).second)
      continue;

    for (auto *succ : successors(BB))
      workList.push_back(succ);
  }
  
  for (auto it = F->begin(), e = F->end(); it != e;) {
    auto *BB = *it++;
    if (std::find(visited.begin(), visited.end(), BB) == visited.end()) {
      ++NumUnreachableBlock;
      deleteBasicBlock(BB);
      changed = true;
    }
  }
  
  return changed;
}

bool SimplifyCFG::runOnFunction(Function *F) {
  bool changed = false;

  bool iterChanged = false;
  // Keep iterating over deleting unreachable code and removing trampolines as
  // long as we are making progress.
  do {
    iterChanged = removeUnreachedBasicBlocks(F);
    changed |= iterChanged;
  } while (iterChanged);

  return changed;
}

std::unique_ptr<Pass> cobra::createSimplifyCFG() {
  return std::make_unique<SimplifyCFG>();
}

#undef DEBUG_TYPE
