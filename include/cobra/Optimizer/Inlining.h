/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Inlining_h
#define Inlining_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class Inlining : public ModulePass {
 public:
  explicit Inlining() : ModulePass("Inlining") {}
  ~Inlining() override = default;

  bool runOnModule(Module *M) override;
};

} // namespace cobra

#endif 
