/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Instrs.h"

using namespace cobra;

bool cobra::isSideEffectFree(Type T) {
  return T.isPrimitive();
}

const char *BinaryOperatorInst::opStringRepr[] = {
    "",   "==", "!=",  "===", "!==", "<", "<=", ">",         ">=",
    "<<", ">>", ">>>", "+",   "-",   "*", "/",  "%",         "|",
    "^",  "&",  "**",  "",    "",    "",  "in", "instanceof"};

const char *BinaryOperatorInst::assignmentOpStringRepr[] = {
    "=",   "",    "",     "",    "",    "",      "",   "",   "",
    "<<=", ">>=", ">>>=", "+=",  "-=",  "*=",    "/=", "%=", "|=",
    "^=",  "&=",  "**=",  "||=", "&&=", "\?\?=", "",   ""};

unsigned TerminatorInst::getNumSuccessors() {
#undef TERMINATOR
#define TERMINATOR(CLASS, PARENT)           \
  if (auto I = dynamic_cast<CLASS *>(this)) \
    return I->getNumSuccessors();
#include "cobra/IR/Instrs.def"
}

//BasicBlock *TerminatorInst::getSuccessor(unsigned idx) const {
//#undef TERMINATOR
//#define TERMINATOR(CLASS, PARENT)           \
//  if (auto I = const_cast<CLASS *>(this)) \
//    return I->getSuccessor(idx);
//#include "cobra/IR/Instrs.def"
//}
//
//void TerminatorInst::setSuccessor(unsigned idx, BasicBlock *B) {
//#undef TERMINATOR
//#define TERMINATOR(CLASS, PARENT)           \
//  if (auto I = const_cast<CLASS *>(this)) \
//    return I->setSuccessor(idx, B);
//#include "cobra/IR/Instrs.def"
//}


static BinaryOperatorInst::OpKind parseOperator_impl(
    StringRef op,
    const char **lookup_table) {
  for (int i = 0; i < static_cast<int>(BinaryOperatorInst::OpKind::LAST_OPCODE);
       i++) {
    if (op == lookup_table[i]) {
      return static_cast<BinaryOperatorInst::OpKind>(i);
    }
  }
}

PhiInst::PhiInst(const ValueListType &values, const BasicBlockListType &blocks)
    : Instruction(ValueKind::PhiInstKind) {
  assert(values.size() == blocks.size() && "Block-value pairs mismatch");
  // Push the incoming values.
  for (int i = 0, e = values.size(); i < e; ++i) {
    pushOperand(values[i]);
    pushOperand(blocks[i]);
  }
}

BinaryOperatorInst::OpKind BinaryOperatorInst::parseOperator(
    StringRef op) {
  return parseOperator_impl(op, opStringRepr);
}

BinaryOperatorInst::OpKind BinaryOperatorInst::parseAssignmentOperator(
    StringRef op) {
  return parseOperator_impl(op, assignmentOpStringRepr);
}

std::optional<BinaryOperatorInst::OpKind>
BinaryOperatorInst::tryGetReverseOperator(BinaryOperatorInst::OpKind op) {
  switch (op) {
    // Commutative operators
    case OpKind::EqualKind:
    case OpKind::NotEqualKind:
    case OpKind::StrictlyEqualKind:
    case OpKind::StrictlyNotEqualKind:
    case OpKind::AddKind:
    case OpKind::MultiplyKind:
    case OpKind::OrKind:
    case OpKind::XorKind:
    case OpKind::AndKind:
      return op;

    // Rewritable operators
    case OpKind::LessThanKind:
      return OpKind::GreaterThanKind;
    case OpKind::LessThanOrEqualKind:
      return OpKind::GreaterThanOrEqualKind;
    case OpKind::GreaterThanKind:
      return OpKind::LessThanKind;
    case OpKind::GreaterThanOrEqualKind:
      return OpKind::LessThanOrEqualKind;

    default:
      return std::nullopt;
  }
}

SideEffectKind
BinaryOperatorInst::getBinarySideEffect(Type leftTy, Type rightTy, OpKind op) {
  // The 'in' and 'instanceof' operators may throw:
  if (op == OpKind::InKind || op == OpKind::InstanceOfKind)
    return SideEffectKind::Unknown;

  // Strict equality does not throw or have other side effects (per ES5 11.9.6).
  if (op == OpKind::StrictlyNotEqualKind || op == OpKind::StrictlyEqualKind)
    return SideEffectKind::None;

  // This instruction does not read/write memory if the LHS and RHS types are
  // known to be safe (number, string, null, etc).
  if (isSideEffectFree(leftTy) && isSideEffectFree(rightTy))
    return SideEffectKind::None;

  // This binary operation may execute arbitrary code.
  return SideEffectKind::Unknown;
}
