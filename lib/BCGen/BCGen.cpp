/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/BCGen.h"
#include "cobra/IR/CFG.h"
#include "cobra/IR/Instrs.h"
#include "cobra/BCGen/BytecodeGenerator.h"
#include "cobra/Optimizer/Pass.h"
#include "cobra/Optimizer/PassManager.h"

#include <algorithm>
#include <queue>

using namespace cobra;

std::unique_ptr<BytecodeModule> generateBytecode(Module *M) {
  BytecodeGenerator BCGen{};
  
  for (auto &F : *M) {
    VirtualRegisterAllocator RA{};
  }
  
}

