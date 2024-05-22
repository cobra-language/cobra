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
#include "cobra/IR/Analysis.h"
#include "cobra/BCGen/BCPasses.h"

#include <algorithm>
#include <queue>

using namespace cobra;

void lowerIR(Module *M) {
  PassManager PM;
  
  PM.addPass<LoadConstants>();
  PM.addPass<LoadParameters>();
  
  PM.run(M);
  
}

std::unique_ptr<BytecodeModule> cobra::generateBytecode(Module *M) {
  lowerIR(M);
  
  BytecodeGenerator BCGen{};
  
  for (auto &F : *M) {
    VirtualRegisterAllocator RA{F};
    
    PostOrderAnalysis PO(F);
    std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
    
    RA.allocate();
    
    F->dump();
    
    auto funcGen = BytecodeFunctionGenerator::create(BCGen, F, RA);
    BCGen.addFunction(F);
    BCGen.setFunctionGenerator(F, std::move(funcGen));
  }
  
  return BCGen.generate();
}

