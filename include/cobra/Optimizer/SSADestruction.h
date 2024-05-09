/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SSADestruction_h
#define SSADestruction_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class SSADestruction : public FunctionPass {
 public:
  explicit SSADestruction() : FunctionPass("SSADestruction") {}
  ~SSADestruction() override = default;

  bool runOnFunction(Function *F) override;
};
} // namespace cobra

#endif
