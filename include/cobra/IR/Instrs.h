/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Instrs_h
#define Instrs_h

#include <stdio.h>
#include "cobra/IR/IR.h"
#include "cobra/Support/Common.h"

namespace cobra {

bool isSideEffectFree(Type T);

class SingleOperandInst : public Instruction {
  SingleOperandInst(const SingleOperandInst &) = delete;
  void operator=(const SingleOperandInst &) = delete;

  // Make pushOperand private to ensure derived classes don't use it.
  using Instruction::pushOperand;

 protected:
  explicit SingleOperandInst(ValueKind K, Value *Op) : Instruction(K) {
    pushOperand(Op);
  }

 public:
  enum { SingleOperandIdx };

  explicit SingleOperandInst(
      const SingleOperandInst *src,
      std::vector<Value *> operands)
      : Instruction(src, operands) {
    assert(operands.size() == 1 && "SingleOperandInst must have 1 operand!");
  }

  Value *getSingleOperand() const {
    return getOperand(0);
  }

  SideEffectKind getSideEffect() {
    
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::SingleOperandInstKind);
  }
};

class TerminatorInst : public Instruction {
  TerminatorInst(const TerminatorInst &) = delete;
  void operator=(const TerminatorInst &) = delete;
  
protected:
 explicit TerminatorInst(ValueKind K) : Instruction(K) {}
  
public:
 explicit TerminatorInst(const TerminatorInst *src, std::vector<Value *> operands)
     : Instruction(src, operands) {}
  
  unsigned getNumSuccessors();
  BasicBlock *getSuccessor(unsigned idx);
  void setSuccessor(unsigned idx, BasicBlock *B);
  
  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::TerminatorInstKind);
  }
};

class BranchInst : public TerminatorInst {
  BranchInst(const BranchInst &) = delete;
  void operator=(const BranchInst &) = delete;

 public:
  enum { BranchDestIdx };

  BasicBlock *getBranchDest() const {
    return dynamic_cast<BasicBlock *>(getOperand(BranchDestIdx));
  }

  explicit BranchInst(BasicBlock *parent, BasicBlock *dest)
      : TerminatorInst(ValueKind::BranchInstKind) {
    pushOperand(dest);
  }
  explicit BranchInst(const BranchInst *src, std::vector<Value *> operands)
      : TerminatorInst(src, operands) {}

  SideEffectKind getSideEffect() {
    return SideEffectKind::None;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::BranchInstKind);
  }

  unsigned getNumSuccessors() const {
    return 1;
  }
  BasicBlock *getSuccessor(unsigned idx) const {
    assert(idx == 0 && "BranchInst only have 1 successor!");
    return getBranchDest();
  }
  void setSuccessor(unsigned idx, BasicBlock *B) {
    assert(idx == 0 && "BranchInst only have 1 successor!");
    setOperand(B, idx);
  }
};

class CondBranchInst : public TerminatorInst {
  CondBranchInst(const CondBranchInst &) = delete;
  void operator=(const CondBranchInst &) = delete;

 public:
  enum { ConditionIdx, TrueBlockIdx, FalseBlockIdx };

  Value *getCondition() const {
    return getOperand(ConditionIdx);
  }
  BasicBlock *getTrueDest() const {
    return dynamic_cast<BasicBlock *>(getOperand(TrueBlockIdx));
  }
  BasicBlock *getFalseDest() const {
    return dynamic_cast<BasicBlock *>(getOperand(FalseBlockIdx));
  }

  explicit CondBranchInst(
      BasicBlock *parent,
      Value *cond,
      BasicBlock *trueBlock,
      BasicBlock *falseBlock)
      : TerminatorInst(ValueKind::CondBranchInstKind) {
    pushOperand(cond);
    pushOperand(trueBlock);
    pushOperand(falseBlock);
  }
  explicit CondBranchInst(
      const CondBranchInst *src,
      std::vector<Value *> operands)
      : TerminatorInst(src, operands) {}

  SideEffectKind getSideEffect() {
    return SideEffectKind::None;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::CondBranchInstKind);
  }

  unsigned getNumSuccessors() const {
    return 2;
  }
  BasicBlock *getSuccessor(unsigned idx) const {
    if (idx == 0)
      return getTrueDest();
    if (idx == 1)
      return getFalseDest();
  }
  void setSuccessor(unsigned idx, BasicBlock *B) {
    assert(idx <= 1 && "CondBranchInst only have 2 successors!");
    setOperand(B, idx + TrueBlockIdx);
  }
};

class ReturnInst : public TerminatorInst {
  ReturnInst(const ReturnInst &) = delete;
  void operator=(const ReturnInst &) = delete;

 public:
  enum { ReturnValueIdx };

  Value *getValue() const {
    return getOperand(ReturnValueIdx);
  }

  explicit ReturnInst(Value *val) : TerminatorInst(ValueKind::ReturnInstKind) {
    pushOperand(val);
  }
  explicit ReturnInst(const ReturnInst *src, std::vector<Value *> operands)
      : TerminatorInst(src, operands) {}

  SideEffectKind getSideEffect() {
    return SideEffectKind::None;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::ReturnInstKind);
  }

  unsigned getNumSuccessors() {
    return 0;
  }
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

class LoadStackInst : public SingleOperandInst {
  LoadStackInst(const LoadStackInst &) = delete;
  void operator=(const LoadStackInst &) = delete;

 public:
  explicit LoadStackInst(AllocStackInst *alloc)
      : SingleOperandInst(ValueKind::LoadStackInstKind, alloc) {}
  explicit LoadStackInst(
      const LoadStackInst *src,
      std::vector<Value *> operands)
      : SingleOperandInst(src, operands) {}

  AllocStackInst *getPtr() const {
    return dynamic_cast<AllocStackInst *>(getSingleOperand());
  }

  SideEffectKind getSideEffect() {
    return SideEffectKind::MayRead;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::LoadStackInstKind);
  }
};

class StoreStackInst : public Instruction {
  StoreStackInst(const StoreStackInst &) = delete;
  void operator=(const StoreStackInst &) = delete;

 public:
  enum { Src, Dest };

  Value *getSrc() const {
    return getOperand(Src);
  }
  AllocStackInst *getDest() const {
    return dynamic_cast<AllocStackInst *>(getOperand(Dest));
  }

  explicit StoreStackInst(Value *src, AllocStackInst *dest)
      : Instruction(ValueKind::StoreStackInstKind) {
    setType(Type::createNoType());
    pushOperand(src);
    pushOperand(dest);
  }

  SideEffectKind getSideEffect() {
    return SideEffectKind::MayWrite;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::StoreStackInstKind);
  }
};

class UnaryOperatorInst : public SingleOperandInst {
 public:
  /// JavaScript Binary operators as defined in the ECMA spec.
  /// http://www.ecma-international.org/ecma-262/7.0/index.html#sec-unary-operators
  enum class OpKind {
    DeleteKind, // delete
    VoidKind, // void
    TypeofKind, // typeof
    PlusKind, // +
    MinusKind, // -
    TildeKind, // ~
    BangKind, // !
    IncKind, // + 1
    DecKind, // - 1
    LAST_OPCODE
  };

 private:
  UnaryOperatorInst(const UnaryOperatorInst &) = delete;
  void operator=(const UnaryOperatorInst &) = delete;

  /// The operator kind.
  OpKind op_;

  // A list of textual representation of the operators above.
  static const char *opStringRepr[(int)OpKind::LAST_OPCODE];

 public:
  /// \return the binary operator kind.
  OpKind getOperatorKind() const {
    return op_;
  }

  // Convert the operator string \p into the enum representation or assert
  // fail if the string is invalud.
  static OpKind parseOperator(StringRef op);

  /// \return the string representation of the operator.
  StringRef getOperatorStr() {
    return opStringRepr[static_cast<int>(op_)];
  }

  explicit UnaryOperatorInst(Value *value, OpKind opKind)
      : SingleOperandInst(ValueKind::UnaryOperatorInstKind, value),
        op_(opKind) {}
  explicit UnaryOperatorInst(
      const UnaryOperatorInst *src,
      std::vector<Value *> operands)
      : SingleOperandInst(src, operands), op_(src->op_) {}

  SideEffectKind getSideEffect();

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::UnaryOperatorInstKind);
  }
};

class BinaryOperatorInst : public Instruction {
 public:
  /// JavaScript Binary operators as defined in the ECMA spec.
  /// http://ecma-international.org/ecma-262/5.1/#sec-11
  enum class OpKind {
    IdentityKind, // nop (assignment operator, no arithmetic op)
    EqualKind, // ==
    NotEqualKind, // !=
    StrictlyEqualKind, // ===
    StrictlyNotEqualKind, // !==
    LessThanKind, // <
    LessThanOrEqualKind, // <=
    GreaterThanKind, // >
    GreaterThanOrEqualKind, // >=
    LeftShiftKind, // <<  (<<=)
    RightShiftKind, // >>  (>>=)
    UnsignedRightShiftKind, // >>> (>>>=)
    AddKind, // +   (+=)
    SubtractKind, // -   (-=)
    MultiplyKind, // *   (*=)
    DivideKind, // /   (/=)
    ModuloKind, // %   (%=)
    OrKind, // |   (|=)
    XorKind, // ^   (^=)
    AndKind, // &   (^=)
    ExponentiationKind, // ** (**=)
    AssignShortCircuitOrKind, // ||= (only for assignment)
    AssignShortCircuitAndKind, // &&= (only for assignment)
    AssignNullishCoalesceKind, // ??= (only for assignment)
    InKind, // "in"
    InstanceOfKind, // instanceof
    LAST_OPCODE
  };

  static const char *opStringRepr[(int)OpKind::LAST_OPCODE];

  static const char *assignmentOpStringRepr[(int)OpKind::LAST_OPCODE];

 private:
  BinaryOperatorInst(const BinaryOperatorInst &) = delete;
  void operator=(const BinaryOperatorInst &) = delete;

  OpKind op_;

 public:
  enum { LeftHandSideIdx, RightHandSideIdx };

  OpKind getOperatorKind() const {
    return op_;
  }

  Value *getLeftHandSide() const {
    return getOperand(LeftHandSideIdx);
  }
  Value *getRightHandSide() const {
    return getOperand(RightHandSideIdx);
  }

  static OpKind parseOperator(StringRef op);

  static OpKind parseAssignmentOperator(StringRef op);

  static std::optional<OpKind> tryGetReverseOperator(OpKind op);

  StringRef getOperatorStr() {
    return opStringRepr[static_cast<int>(op_)];
  }

  explicit BinaryOperatorInst(Value *left, Value *right, OpKind opKind)
      : Instruction(ValueKind::BinaryOperatorInstKind), op_(opKind) {
    pushOperand(left);
    pushOperand(right);
  }
  explicit BinaryOperatorInst(
      const BinaryOperatorInst *src,
      std::vector<Value *> operands)
      : Instruction(src, operands), op_(src->op_) {}

  SideEffectKind getSideEffect() {
    return getBinarySideEffect(
        getLeftHandSide()->getType(),
        getRightHandSide()->getType(),
        getOperatorKind());
  }


  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::BinaryOperatorInstKind);
  }

  static SideEffectKind
  getBinarySideEffect(Type leftTy, Type rightTy, OpKind op);
};

class PhiInst : public Instruction {
  PhiInst(const PhiInst &) = delete;
  void operator=(const PhiInst &) = delete;

 public:
  using ValueListType = std::vector<Value *>;
  using BasicBlockListType = std::vector<BasicBlock *>;

  unsigned getNumEntries() const;

  std::pair<Value *, BasicBlock *> getEntry(unsigned i) const;

  void updateEntry(unsigned i, Value *val, BasicBlock *BB);

  void addEntry(Value *val, BasicBlock *BB);

  void removeEntry(unsigned index);

  void removeEntry(BasicBlock *BB);

  explicit PhiInst(
      const ValueListType &values,
      const BasicBlockListType &blocks);
  explicit PhiInst(const PhiInst *src, std::vector<Value *> operands)
      : Instruction(src, operands) {}

  SideEffectKind getSideEffect() {
    return SideEffectKind::None;
  }

  static bool classof(const Value *V) {
    return kindIsA(V->getKind(), ValueKind::PhiInstKind);
  }
};

}

#endif /* Instrs_h */
