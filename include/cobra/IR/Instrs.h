/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Instrs_hpp
#define Instrs_hpp

#include <stdio.h>
#include "cobra/IR/IR.h"

namespace cobra {

class SingleOperandInst : public Instruction {
  SingleOperandInst(const SingleOperandInst &) = delete;
  void operator=(const SingleOperandInst &) = delete;
  
};

class TerminatorInst : public Instruction {
  TerminatorInst(const TerminatorInst &) = delete;
  void operator=(const TerminatorInst &) = delete;
  
protected:
 explicit TerminatorInst(ValueKind K) : Instruction(K) {}
  
};

}

#endif /* Instrs_hpp */
