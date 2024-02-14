/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef DCE_h
#define DCE_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class DCE : public ModulePass {
 public:
  explicit DCE() : ModulePass("DCE") {}
  ~DCE() override = default;

  bool runOnModule(Module *M) override;
};

} // namespace cobra

#endif
