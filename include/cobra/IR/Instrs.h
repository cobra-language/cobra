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

class AllocStackInst : public Instruction {
  AllocStackInst(const AllocStackInst &) = delete;
  void operator=(const AllocStackInst &) = delete;

  Label variableName;

 public:
  enum { VariableNameIdx };

  explicit AllocStackInst(Identifier varName)
      : Instruction(ValueKind::AllocStackInstKind), variableName(varName) {
    pushOperand(&variableName);
  }
  explicit AllocStackInst(
      const AllocStackInst *src,
      std::vector<Value *> operands)
      : AllocStackInst(dynamic_cast<Label *>(operands[0])->get()) {
    // NOTE: we are playing a little trick here since the Label is not heap
    // allocated.
  }

  Identifier getVariableName() const {
    return variableName.get();
  }

  SideEffectKind getSideEffect() {
    return SideEffectKind::None;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::AllocStackInstKind);
  }
};

class StoreStackInst : public Instruction {
  StoreStackInst(const StoreStackInst &) = delete;
  void operator=(const StoreStackInst &) = delete;

 public:
  enum { StoredValueIdx, PtrIdx };

  Value *getValue() const {
    return getOperand(StoredValueIdx);
  }
  AllocStackInst *getPtr() const {
    return dynamic_cast<AllocStackInst *>(getOperand(PtrIdx));
  }

  explicit StoreStackInst(Value *storedValue, AllocStackInst *ptr)
      : Instruction(ValueKind::StoreStackInstKind) {
    setType(Type::createNoType());
    pushOperand(storedValue);
    pushOperand(ptr);
  }

  SideEffectKind getSideEffect() {
    return SideEffectKind::MayWrite;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::StoreStackInstKind);
  }
};

class TerminatorInst : public Instruction {
  TerminatorInst(const TerminatorInst &) = delete;
  void operator=(const TerminatorInst &) = delete;
  
protected:
 explicit TerminatorInst(ValueKind K) : Instruction(K) {}
  
};

}

#endif /* Instrs_hpp */
