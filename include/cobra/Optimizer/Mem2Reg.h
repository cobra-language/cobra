/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Mem2Reg_h
#define Mem2Reg_h

#include "cobra/IR/IR.h"
#include "cobra/Optimizer/Pass.h"

namespace cobra {

/// This optimization promotes stack allocations into virtual registers.
/// The algorithm is based on:
///
///  Sreedhar and Gao. A linear time algorithm for placing phi-nodes. POPL '95.
class Mem2Reg : public FunctionPass {
 public:
  explicit Mem2Reg() : FunctionPass("Mem2Reg") {}
  ~Mem2Reg() override = default;

  bool runOnFunction(Function *F) override;
};

} // namespace cobra

#endif 
