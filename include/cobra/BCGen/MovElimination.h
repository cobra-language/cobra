/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef MovElimination_h
#define MovElimination_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

class VirtualRegisterAllocator;

class MovElimination : public FunctionPass {
 public:
  explicit MovElimination(VirtualRegisterAllocator &RA)
      : FunctionPass("MovElimination"), RA_(RA) {}
  ~MovElimination() override = default;

  bool runOnFunction(Function *F) override;

 private:
  VirtualRegisterAllocator &RA_;
};

}

#endif
