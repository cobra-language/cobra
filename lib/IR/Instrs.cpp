/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Instrs.h"

using namespace cobra;

BasicBlock *TerminatorInst::getSuccessor(unsigned idx) {
#undef TERMINATOR
#define TERMINATOR(CLASS, PARENT)           \
  if (auto I = static_cast<CLASS *>(this)) \
    return I->getSuccessor(idx);
#include "cobra/IR/Instrs.def"
}

void TerminatorInst::setSuccessor(unsigned idx, BasicBlock *B) {
#undef TERMINATOR
#define TERMINATOR(CLASS, PARENT)           \
  if (auto I = static_cast<CLASS *>(this)) \
    return I->setSuccessor(idx, B);
#include "cobra/IR/Instrs.def"
}

bool cobra::isSideEffectFree(Type T) {
  return T.isPrimitive();
}

const char *UnaryOperatorInst::opStringRepr[] =
    {"delete", "void", "typeof", "+", "-", "~", "!", "++", "--"};

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
  if (auto I = static_cast<CLASS *>(this)) \
    return I->getNumSuccessors();
#include "cobra/IR/Instrs.def"
  COBRA_UNREACHABLE();
}

UnaryOperatorInst::OpKind UnaryOperatorInst::parseOperator(StringRef op) {
  for (int i = 0; i < static_cast<int>(BinaryOperatorInst::OpKind::LAST_OPCODE);
       i++) {
    if (op == UnaryOperatorInst::opStringRepr[i]) {
      return static_cast<UnaryOperatorInst::OpKind>(i);
    }
  }
  
  COBRA_UNREACHABLE();
}

SideEffectKind UnaryOperatorInst::getSideEffect() {
  if (getOperatorKind() == OpKind::DeleteKind) {
    return SideEffectKind::Unknown;
  }

  if (isSideEffectFree(getSingleOperand()->getType())) {
    return SideEffectKind::None;
  }

  switch (getOperatorKind()) {
    case OpKind::VoidKind: // void
    case OpKind::TypeofKind: // typeof
      return SideEffectKind::None;

    default:
      break;
  }

  return SideEffectKind::Unknown;
}

static BinaryOperatorInst::OpKind parseOperator_impl(
    StringRef op,
    const char **lookup_table) {
  for (int i = 0; i < static_cast<int>(BinaryOperatorInst::OpKind::LAST_OPCODE);
       i++) {
    if (op == lookup_table[i]) {
      return static_cast<BinaryOperatorInst::OpKind>(i);
    }
  }
  
  COBRA_UNREACHABLE();
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

unsigned PhiInst::getNumEntries() const {
  // The PHI operands are just pairs of values and basic blocks.
  return getNumOperands() / 2;
}

static unsigned indexOfPhiEntry(unsigned index) {
  return index * 2;
}

std::pair<Value *, BasicBlock *> PhiInst::getEntry(unsigned i) const {
  return std::make_pair(
      getOperand(indexOfPhiEntry(i)),
      dynamic_cast<BasicBlock *>(getOperand(indexOfPhiEntry(i) + 1)));
}

void PhiInst::removeEntry(unsigned index) {
  unsigned startIdx = indexOfPhiEntry(index);
  // Remove the value:
  removeOperand(startIdx);
  // Remove the basic block. Notice that we use the same index because the
  // list is shifted.
  removeOperand(startIdx);
}

void PhiInst::removeEntry(BasicBlock *BB) {
  unsigned i = 0;
  // For each one of the entries:
  while (i < getNumEntries()) {
    // If this entry is from the BB we want to remove, then remove it.
    if (getEntry(i).second == BB) {
      removeEntry(i);
      // keep the current iteration index.
      continue;
    }
    // Else, move to the next entry.
    i++;
  }
}
