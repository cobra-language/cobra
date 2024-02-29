/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/Optimizer/Pipeline.h"
#include "cobra/Optimizer/PassManager.h"
#include "cobra/Optimizer/DCE.h"
#include "cobra/Optimizer/SimplifyCFG.h"


#define DEBUG_TYPE "pipeline"

void cobra::runFullOptimizationPasses(Module &M) {
  PassManager PM;
  PM.addSimplifyCFG();
  PM.addMem2Reg();
  

  PM.run(&M);
}

#undef DEBUG_TYPE
