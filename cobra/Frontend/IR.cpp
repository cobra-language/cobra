/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "IR.hpp"

using namespace cobra;

void Value::destroy(Value *V) {
  return;
  
//  if (!V)
//    return;
//
//  switch (V->Kind) {
//    default:
//      break;;
//#define DEF_VALUE(XX, PARENT) \
//  case ValueKind::XX##Kind:   \
//    delete cast<XX>(V);       \
//    break;
//#include "ValueKinds.def"
//  }
}

bool Value::hasOneUser() const {
    return 1 == Users.size();
}

void Value::removeUse(Use U) {
    assert(Users.size() && "Removing a user from an empty list");
    assert(U.first == this && "Invalid user");

    Users[U.second] = Users.back();
    Users.pop_back();

    if (U.second != Users.size()) {
        Use oldUse = {this, Users.size()};
        auto &operands = Users[U.second]->Operands;
        for (int i = 0, e = operands.size(); i < e; i++) {
            if (operands[i] == oldUse) {
                operands[i] = {this, U.second};
                return;
            }
        }
    }
}

Value::Use Value::addUser(Instruction *Inst) {
    Users.push_back(Inst);
    return {this, Users.size() - 1};
}

/// Replaces all uses of the current value with \p Other.
void Value::replaceAllUsesWith(Value *Other) {
    if (this == Other) {
        return;
    }

    // Ask the users of this value to unregister themselves. Notice that the users
    // modify and invalidate the iterators of Users.
    while (Users.size()) {
        Users[Users.size() - 1]->replaceFirstOperandWith(this, Other);
    }
}

Parameter::Parameter(Function *parent, std::string name)
    : Value(ValueKind::ParameterKind), Parent(parent), Name(std::move(name)) {
  Parent->addParameter(this);
}

void Instruction::pushOperand(Value *Val) {
  Operands.push_back({nullptr, 0});
  setOperand(Val, getNumOperands() - 1);
}

void Instruction::setOperand(Value *Val, unsigned Index) {
  assert(Index < Operands.size() && "Not all operands have been pushed!");

  Value *CurrentValue = Operands[Index].first;

  // If the operand is already set then there is nothing to do. The instruction
  // is already registered in the use-list of the value.
  if (CurrentValue == Val) {
    return;
  }

  // Remove the current instruction from the old value that we are removing.
  if (CurrentValue) {
    CurrentValue->removeUse(Operands[Index]);
  }

  // Register this instruction as a user of the new value and set the operand.
  if (Val) {
    Operands[Index] = Val->addUser(this);
  } else {
    Operands[Index] = {nullptr, 0};
  }
}

Value *Instruction::getOperand(unsigned Index) const {
  return Operands[Index].first;
}

unsigned Instruction::getNumOperands() const {
  return Operands.size();
}

void Instruction::removeOperand(unsigned index) {
  // We call to setOperand before deleting the operand because setOperand
  // un-registers the user from the user list.
  setOperand(nullptr, index);
  Operands.erase(Operands.begin() + index);
}

void Instruction::eraseFromParent() {
  // Release this instruction from the use-list of other instructions.
  for (unsigned i = 0; i < getNumOperands(); i++)
    setOperand(nullptr, i);

  getParent()->erase(this);
}

void Instruction::replaceFirstOperandWith(Value *OldValue, Value *NewValue) {
  for (int i = 0, e = getNumOperands(); i < e; i++) {
    if (OldValue == getOperand(i)) {
      setOperand(NewValue, i);
      return;
    }
  }
}

BasicBlock::BasicBlock(Function *parent) : Value(ValueKind::BasicBlockKind), Parent(parent) {
  assert(Parent && "Invalid parent function");
  Parent->addBlock(this);
}

void BasicBlock::push_back(Instruction *I) {
  InstList.push_back(I);
}

void BasicBlock::remove(Instruction *I) {
  InstList.remove(I);
}

void BasicBlock::erase(Instruction *I) {
  for (auto it = begin(), e = end(); it != e; it++) {
    if (I == *it) {
      InstList.erase(it);
    }
  }
}

void BasicBlock::eraseFromParent() {
  while (begin() != end()) {
    (*begin())->replaceAllUsesWith(nullptr);
    (*begin())->eraseFromParent();
  }
  
  auto BlockList = getParent()->getBasicBlockList();
  for (auto it = BlockList.begin(), e = BlockList.end(); it != e; it++) {
    if (this == *it) {
      BlockList.erase(it);
    }
  }
}

Function::~Function() {
  for (auto *p : Parameters) {
    Value::destroy(p);
  }
}

void Function::addBlock(BasicBlock *BB) {
  BasicBlockList.push_back(BB);
}
void Function::addParameter(Parameter *A) {
  Parameters.push_back(A);
}

Module::~Module() {
  FunctionList.clear();
  
}


void Module::push_back(Function *F) {
    FunctionList.push_back(F);
}
