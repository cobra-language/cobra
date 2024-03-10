/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#define DEBUG_TYPE "dce"

#include "cobra/Optimizer/DCE.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"
#include "cobra/IR/Analysis.h"

using namespace cobra;

static bool performFunctionDCE(Function *F) {
  bool changed = false;
  F->dump();
  PostOrderAnalysis PO(F);
  F->dump();
  
  IRBuilder::InstructionDestroyer destroyer;
  
  for (auto *BB : PO) {
    // Scan the instructions in the block from end to start.
    for (auto it = BB->rbegin(), e = BB->rend(); it != e; /* nothing */) {
      Instruction *I = *it;
      // Move the iterator to the next instruction in the block. This will
      // allow us to delete the current instruction.
      ++it;

      // If the instruction writes to memory then we can't remove it. Notice
      // that it is okay to delete instructions that only read memory and are
      // unused.
      //
      // Terminators don't have any uses but are never supposed to be removed
      // as dead code.
      //
      // CreateScopeInst may not have any users, but it is lowered to
      // HBCCreateEnvironmentInst which should always be emitted and DCE'd if
      // appropriate.
      //
      // HasRestrictedGlobalPropertyInst doesn't have a result but should never
      // be removed as they perform runtime validation.
      if (I->mayWriteMemory() || dynamic_cast<TerminatorInst *>(I)) {
        continue;
      }

      // If some other instruction is using the result of this instruction then
      // we can't delete it.
      if (I->getNumUsers())
        continue;

      destroyer.add(I);
      changed = true;
    }
  }
  
  return changed;
}

bool DCE::runOnModule(Module *M) {
  bool changed = false;

  for (auto &F : *M) {
    F->dump();
    changed |= performFunctionDCE(F);
    F->dump();
  }

  return changed;
}

std::unique_ptr<Pass> cobra::createDCE() {
  return std::make_unique<DCE>();
}

#undef DEBUG_TYPE
