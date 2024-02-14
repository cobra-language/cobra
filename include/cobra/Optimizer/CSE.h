/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CSE_h
#define CSE_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class CSE : public FunctionPass {
 public:
  explicit CSE() : FunctionPass("CSE") {}
  ~CSE() override = default;

  bool runOnFunction(Function *F) override;
};

} // namespace cobra

#endif
