/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef SimplifyCFG_h
#define SimplifyCFG_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class SimplifyCFG : public FunctionPass {
 public:
  explicit SimplifyCFG() : FunctionPass("SimplifyCFG") {}
  ~SimplifyCFG() override = default;

  bool runOnFunction(Function *F) override;
};
} // namespace cobra

#endif
