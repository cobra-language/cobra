/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BCPasses_h
#define BCPasses_h


#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class LoadConstants : public FunctionPass {

 public:
  explicit LoadConstants() : FunctionPass("LoadConstants") {}
  ~LoadConstants() override = default;

  bool runOnFunction(Function *F) override;
};

class LoadParameters : public FunctionPass {
 public:
  explicit LoadParameters() : FunctionPass("LoadParameters") {}
  ~LoadParameters() override = default;

  bool runOnFunction(Function *F) override;
};

}

#endif
