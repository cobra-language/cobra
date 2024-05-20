/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/BytecodeGenerator.h"
#include "cobra/Support/Common.h"

using namespace cobra;

static constexpr param_t JumpTempValue = 0;

void BytecodeFunctionGenerator::addJumpToRelocations(offset_t loc, BasicBlock *target) {
  relocations_.push_back({loc, Relocation::RelocationType::LongJumpType, target});
}

void BytecodeFunctionGenerator::resolveRelocations() {
  
}

unsigned BytecodeFunctionGenerator::encodeValue(Value *value) {
  if (dynamic_cast<Instruction *>(value)) {
    return RA_.getRegister(value).getIndex();
  } else if (auto *var = dynamic_cast<Variable *>(value)) {
    return var->getIndexInVariableList();
  } else {
    COBRA_UNREACHABLE();
  }
}

void BytecodeFunctionGenerator::emitMovIfNeeded(param_t dest, param_t src) {
  if (dest == src)
    return;
  
  this->emitMov(dest, src);
}

void BytecodeFunctionGenerator::generateSingleOperandInst(SingleOperandInst *Inst, BasicBlock *next) {
  COBRA_UNREACHABLE();
}

void BytecodeFunctionGenerator::generateLoadStackInst(LoadStackInst *Inst, BasicBlock *next) {
  auto dst = encodeValue(Inst);
  auto src = encodeValue(Inst->getSingleOperand());
  emitMovIfNeeded(dst, src);
}

void BytecodeFunctionGenerator::generateMovInst(MovInst *Inst, BasicBlock *next) {
  auto dst = encodeValue(Inst);
  auto src = encodeValue(Inst->getSingleOperand());
  emitMovIfNeeded(dst, src);
}

void BytecodeFunctionGenerator::generateUnaryOperatorInst(UnaryOperatorInst *Inst, BasicBlock *next) {
  
}

void BytecodeFunctionGenerator::generatePhiInst(PhiInst *Inst, BasicBlock *next) {
  // PhiInst has been translated into a sequence of MOVs in RegAlloc
  // Nothing to do here.
}

void BytecodeFunctionGenerator::generateBinaryOperatorInst(BinaryOperatorInst *Inst, BasicBlock *next) {
  auto left = encodeValue(Inst->getLeftHandSide());
  auto right = encodeValue(Inst->getRightHandSide());
  auto res = encodeValue(Inst);
  
  using OpKind = BinaryOperatorInst::OpKind;
  
  switch (Inst->getOperatorKind()) {
    case OpKind::EqualKind: // ==
      this->emitEq(res, left, right);
      break;
    case OpKind::ModuloKind: // %   (%=)
      this->emitMod(res, left, right);
      break;
    case OpKind::AddKind: // +   (+=)
      this->emitAdd(res, left, right);
      break;
    default:
      break;
  }
}

void BytecodeFunctionGenerator::generateStoreStackInst(StoreStackInst *Inst, BasicBlock *next) {
  COBRA_UNREACHABLE();
}

void BytecodeFunctionGenerator::generateAllocStackInst(AllocStackInst *Inst, BasicBlock *next) {
  COBRA_UNREACHABLE();
}

void BytecodeFunctionGenerator::generateTerminatorInst(TerminatorInst *Inst, BasicBlock *next) {
  COBRA_UNREACHABLE();
}

void BytecodeFunctionGenerator::generateBranchInst(BranchInst *Inst, BasicBlock *next) {
  auto *dst = Inst->getBranchDest();
  if (dst == next)
    return;
  
  auto loc = this->emitJmpLong(JumpTempValue);
  addJumpToRelocations(loc, dst);
}

void BytecodeFunctionGenerator::generateReturnInst(ReturnInst *Inst, BasicBlock *next) {
  auto value = encodeValue(Inst->getValue());
  this->emitRet(value);
}

void BytecodeFunctionGenerator::generateCondBranchInst(CondBranchInst *Inst, BasicBlock *next) {
  auto condReg = encodeValue(Inst->getCondition());

  BasicBlock *trueBlock = Inst->getTrueDest();
  BasicBlock *falseBlock = Inst->getFalseDest();
  
  if (next == trueBlock) {
    auto loc = this->emitJmpFalseLong(JumpTempValue, condReg);
    addJumpToRelocations(loc, falseBlock);
    return;
  }
  
  auto loc = this->emitJmpTrueLong(JumpTempValue, condReg);
  addJumpToRelocations(loc, trueBlock);
  
  if (next == falseBlock) {
    return;
  }
  
  loc = this->emitJmpFalseLong(JumpTempValue, condReg);
  addJumpToRelocations(loc, falseBlock);
}

void BytecodeFunctionGenerator::generate(Instruction *ii, BasicBlock *next) {
  
}


std::unique_ptr<BytecodeFunction>
BytecodeFunctionGenerator::generateBytecodeFunction() {
  
}

unsigned BytecodeGenerator::addFunction(Function *F) {
  functions.push_back(F);
  unsigned index = functions.size();
  functionIDMap[F] = index;
  return index;
}

void BytecodeGenerator::setFunctionGenerator(
    Function *F,
    std::unique_ptr<BytecodeFunctionGenerator> BFG) {
  assert(
      functionGenerators_.find(F) == functionGenerators_.end() &&
      "Adding same function twice.");
  assert(
      !BFG->hasEncodingError() && "Error should have been reported already.");
  functionGenerators_[F] = std::move(BFG);
}

std::unique_ptr<BytecodeModule> BytecodeGenerator::generate() {
  std::unique_ptr<BytecodeModule> BM{new BytecodeModule()};
  
  for (unsigned i = 0, e = functions.size(); i < e; ++i) {
    auto *F = functions[i];
    auto &BFG = *functionGenerators_[F];
    
    std::unique_ptr<BytecodeFunction> func = BFG.generateBytecodeFunction();
    
    BM->setFunction(i, std::move(func));
  }
  return BM;
}