/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/BytecodeGenerator.h"
#include "cobra/Support/Common.h"
#include "cobra/IR/Analysis.h"

using namespace cobra;

static constexpr param_t JumpTempValue = 0;

void BytecodeFunctionGenerator::addJumpToRelocations(offset_t loc, BasicBlock *target) {
  relocations_.push_back({loc, Relocation::RelocationType::LongJumpType, target});
}

void BytecodeFunctionGenerator::generateJumpTable() {
  // TODO
}

void BytecodeFunctionGenerator::resolveRelocations() {
  bool changed;
  do {
    int totalShift = 0;
    changed = false;
    for (auto &relocation : relocations_) {
      auto loc = relocation.loc;
      auto *pointer = relocation.pointer;
      auto type = relocation.type;
      loc -= totalShift;
      relocation.loc = loc;
      switch (type) {
        case Relocation::LongJumpType: {
          int targetLoc = basicBlockMap_[dynamic_cast<BasicBlock *>(pointer)].first;
          int jumpOffset = targetLoc - loc;
          if (-128 <= jumpOffset && jumpOffset < 128) {
            // The jump offset can fit into one byte.
            totalShift += 3;
            this->shrinkJump(loc + 1);
            this->updateJumpTarget(loc + 1, jumpOffset, 1);
            relocation.type = Relocation::JumpType;
            changed = true;
          } else {
            this->updateJumpTarget(loc + 1, jumpOffset, 4);
          }
          break;
        }
        case Relocation::BasicBlockType:
          basicBlockMap_[dynamic_cast<BasicBlock *>(pointer)].first = loc;
          break;
        case Relocation::JumpType: {
          int targetLoc = basicBlockMap_[dynamic_cast<BasicBlock *>(pointer)].first;
          int jumpOffset = targetLoc - loc;
          this->updateJumpTarget(loc + 1, jumpOffset, 1);
          break;
        }
        default:
          break;
      }
    }
  } while (changed);
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

void BytecodeFunctionGenerator::generateLoadConstInst(LoadConstInst *Inst, BasicBlock *next) {
  auto output = encodeValue(Inst);
  Literal *literal = Inst->getConst();
  switch (literal->getKind()) {
    case ValueKind::LiteralNumberKind: {
      auto *litNum = dynamic_cast<LiteralNumber *>(literal);
      if (litNum->isPositiveZero()) {
        this->emitLoadConstZero(output);
      } else if (litNum->isUInt8Representible()) {
        this->emitLoadConstUInt8(output, litNum->asUInt8());
      } else {
        // param_t is int64_t, we cannot directly convert a double into that.
        // Instead we are going to copy it as if it is binary.
        this->emitLoadConstDoubleDirect(output, litNum->getValue());
      }
      break;
    }
    default:
      COBRA_UNREACHABLE();
  }
}

void BytecodeFunctionGenerator::generateLoadParamInst(LoadParamInst *Inst, BasicBlock *next) {
  auto output = encodeValue(Inst);
  LiteralNumber *number = Inst->getIndex();
  auto value = number->asUInt32();
  this->emitLoadParam(output, value);
}

void BytecodeFunctionGenerator::generateBody() {
  PostOrderAnalysis PO(F_);
  std::vector<BasicBlock *> order(PO.rbegin(), PO.rend());
  
  for (int i = 0, e = order.size(); i < e; ++i) {
    BasicBlock *BB = order[i];
    BasicBlock *next = ((i + 1) == e) ? nullptr : order[i + 1];
    generateCodeBlock(BB, next);
  }
  
  resolveRelocations();
}

void BytecodeFunctionGenerator::generateCodeBlock(BasicBlock *BB, BasicBlock *next) {
  auto begin_loc = this->getCurrentLocation();
  
  relocations_.push_back({begin_loc, Relocation::RelocationType::BasicBlockType, BB});
  basicBlockMap_[BB] = std::make_pair(begin_loc, next);
  
  BB->dump();
  
  for (auto &I : *BB) {
    generateInst(I, next);
  }
  
  auto end_loc = this->getCurrentLocation();
  if (!next) {
    // When next is nullptr, we are hitting the last BB.
    // We should also register that null BB with it's location.
    assert(
        basicBlockMap_.find(nullptr) == basicBlockMap_.end() &&
        "Multiple nullptr BBs encountered");
    basicBlockMap_[nullptr] = std::make_pair(end_loc, nullptr);
  }
}

void BytecodeFunctionGenerator::generateInst(Instruction *ii, BasicBlock *next) {
  switch (ii->getKind()) {
#define DEF_VALUE(CLASS, PARENT) \
  case ValueKind::CLASS##Kind:   \
    return generate##CLASS(dynamic_cast<CLASS *>(ii), next);
#include "cobra/IR/Instrs.def"

    default:
      COBRA_UNREACHABLE();
  }
}

std::unique_ptr<BytecodeFunction>
BytecodeFunctionGenerator::generateBytecodeFunction() {
  return std::make_unique<BytecodeFunction>(std::move(opcodes_));
}

unsigned BytecodeGenerator::addFunction(Function *F) {
  functions.push_back(F);
  unsigned index = functions.size();
  functionIDMap[F] = index;
  return index;
}

void BytecodeFunctionGenerator::shrinkJump(offset_t loc) {
  // We are shrinking a long jump into a short jump.
  // The size of operand reduces from 4 bytes to 1 byte, a delta of 3.
  opcodes_.erase(opcodes_.begin() + loc, opcodes_.begin() + loc + 3);

  // Change this instruction from long jump to short jump.
  longToShortJump(loc - 1);
}

void BytecodeFunctionGenerator::updateJumpTarget(
    offset_t loc,
    int newVal,
    int bytes) {
  // The jump target is encoded in little-endian. Update it correctly
  // regardless of host byte order.
  for (; bytes; --bytes, ++loc) {
    opcodes_[loc] = (opcode_t)(newVal);
    newVal >>= 8;
  }
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
  std::unique_ptr<BytecodeModule> BM{new BytecodeModule(functionGenerators_.size())};
  
  for (unsigned i = 0, e = functions.size(); i < e; ++i) {
    auto *F = functions[i];
    auto &BFG = *functionGenerators_[F];
    
    std::unique_ptr<BytecodeFunction> func = BFG.generateBytecodeFunction();
    
    BM->setFunction(i, std::move(func));
  }
  return BM;
}
