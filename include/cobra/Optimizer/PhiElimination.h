/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef PhiElimination_h
#define PhiElimination_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class PhiElimination : public FunctionPass {
 public:
  explicit PhiElimination() : FunctionPass("PhiElimination") {}
  ~PhiElimination() override = default;

  bool runOnFunction(Function *F) override;
};
} // namespace cobra

#endif
