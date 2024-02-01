/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/IRBuilder.h"

using namespace cobra;

BasicBlock *IRBuilder::createBasicBlock(Function *Parent) {
  return new BasicBlock(Parent);
}

Function *IRBuilder::createFunction(Identifier OriginalName) {
  return new Function(M, OriginalName);
}

Parameter *IRBuilder::createParameter(Function *Parent, Identifier Name) {
  return new Parameter(Parent, Name);
}

Variable *IRBuilder::createVariable(Variable::DeclKind declKind, Identifier Name) {
  return new Variable(declKind, Name);
}

LiteralNumber *IRBuilder::getLiteralNumber(double value) {
  auto New = new LiteralNumber(value);
  return New;
}

LiteralString *IRBuilder::getLiteralString(StringRef value) {
  Identifier Iden = createIdentifier(value);
  auto New = new LiteralString(Iden);
  return New;
}

LiteralBool *IRBuilder::getLiteralBool(bool value) {
  if (value)
    return &literalTrue;
  return &literalFalse;
}

LiteralEmpty *IRBuilder::getLiteralEmpty() {
  return &literalEmpty;
}

LiteralUndefined *IRBuilder::getLiteralUndefined() {
  return &literalUndefined;
}

LiteralNull *IRBuilder::getLiteralNull() {
  return &literalNull;
}

Identifier IRBuilder::createIdentifier(StringRef str) {
  return M->getContext().getIdentifier(str);
}

void IRBuilder::insert(Instruction *Inst) {
  assert(!Inst->getParent() && "Instr that's already inserted elsewhere");
  Inst->setLocation(Location);
  Inst->setParent(Block);
  Block->insert(InsertionPoint, Inst);
}

void IRBuilder::setInsertionBlock(BasicBlock *BB) {
  if (BB) {
    InsertionPoint = BB->end();
    Block = BB;
  } else {
    InsertionPoint = BasicBlock::iterator();
    Block = nullptr;
  }
}

BasicBlock *IRBuilder::getInsertionBlock() {
  return Block;
}

void IRBuilder::setInsertionPoint(Instruction *IP) {
  InsertionPoint = IP->getParent()->getIterator(IP);
  Block = IP->getParent();
}

BranchInst *IRBuilder::createBranchInst(BasicBlock *Destination) {
  auto *BI = new BranchInst(getInsertionBlock(), Destination);
  insert(BI);
  return BI;
}

CondBranchInst *
IRBuilder::createCondBranchInst(Value *Cond, BasicBlock *T, BasicBlock *F) {
  auto *CBI = new CondBranchInst(getInsertionBlock(), Cond, T, F);
  insert(CBI);
  return CBI;
}

ReturnInst *IRBuilder::createReturnInst(Value *Val) {
  auto *RI = new ReturnInst(Val);
  insert(RI);
  return RI;
}

AllocStackInst *IRBuilder::createAllocStackInst(Identifier varName) {
  auto *AHI = new AllocStackInst(varName);
  insert(AHI);
  return AHI;
}

LoadStackInst *IRBuilder::createLoadStackInst(AllocStackInst *ptr) {
  auto LI = new LoadStackInst(ptr);
  insert(LI);
  return LI;
}

StoreStackInst *IRBuilder::createStoreStackInst(Value *storedValue, AllocStackInst *ptr) {
  auto SI = new StoreStackInst(storedValue, ptr);
  insert(SI);
  return SI;
}

BinaryOperatorInst *IRBuilder::createBinaryOperatorInst(
    Value *left,
    Value *right,
    BinaryOperatorInst::OpKind opKind) {
  auto BOI = new BinaryOperatorInst(left, right, opKind);
  insert(BOI);
  return BOI;
}

PhiInst *IRBuilder::createPhiInst(
    const PhiInst::ValueListType &values,
    const PhiInst::BasicBlockListType &blocks) {
  auto PI = new PhiInst(values, blocks);
  insert(PI);
  return PI;
}
