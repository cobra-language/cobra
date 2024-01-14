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
