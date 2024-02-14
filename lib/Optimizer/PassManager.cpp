/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/Optimizer/PassManager.h"
#include "cobra/IR/IR.h"

#include <string>

#define DEBUG_TYPE "passmanager"

using namespace cobra;

PassManager::~PassManager() = default;

void PassManager::addPass(std::unique_ptr<Pass> P) {
  pipeline_.emplace_back(std::move(P));
}

void PassManager::run(Module *M) {
  for (std::unique_ptr<Pass> &P : pipeline_) {
    if (auto *FP = dynamic_cast<FunctionPass *>(P.get())) {;
      for (auto F : M->getFunctionList()) {
        FP->runOnFunction(F);
      }
      M->dump();
      // Move to the next pass.
      continue;
    }
  }
}

void PassManager::run(Function *F) {

  // For each pass:
  for (const std::unique_ptr<Pass> &P : pipeline_) {
    auto *FP = dynamic_cast<FunctionPass *>(P.get());
    assert(FP && "Invalid pass kind");
    FP->runOnFunction(F);
  }
}

#undef DEBUG_TYPE
