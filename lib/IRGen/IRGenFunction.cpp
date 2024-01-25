/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace cobra;
using namespace Lowering;

void TreeIRGen::emitFunction(FuncDecl *fd) {
  Identifier functionName = getNameFieldFromID(fd->id);
  std::cout << functionName.c_str() << std::endl;
  Function *newFunction = Builder.createFunction(functionName);
  this->curFunction = newFunction;
  
  emitFunctionPreamble(Builder.createBasicBlock(newFunction));
  
  emitParameters(fd);
    
  emitStatement(fd->body);
  
  emitFunctionEpilogue(Builder.getLiteralUndefined());
}

void TreeIRGen::emitFunctionPreamble(BasicBlock *entry) {
  BasicBlock *realEntry = this->curFunction->front();
  if (realEntry->empty()) {
    Builder.setInsertionBlock(realEntry);
  }else {
    Builder.setInsertionPoint(realEntry->front());
  }
  
  Builder.setInsertionBlock(entry);
}

void TreeIRGen::emitParameters(AbstractFunctionDecl *funcNode) {
  for (auto paramDecl : funcNode->params) {
    auto paramName = getNameFieldFromID(paramDecl->id);
    std::cout << paramName.c_str() << std::endl;
    
    auto *param = Builder.createParameter(this->curFunction, paramName);
    
    if (paramDecl->init) {
      auto *currentBlock = Builder.getInsertionBlock();
      auto *getDefaultBlock = Builder.createBasicBlock(Builder.getFunction());
      auto *storeBlock = Builder.createBasicBlock(Builder.getFunction());
      
      Builder.createCondBranchInst(
          Builder.createBinaryOperatorInst(
              param,
              Builder.getLiteralUndefined(),
              BinaryOperatorInst::OpKind::StrictlyNotEqualKind),
          storeBlock,
          getDefaultBlock);
      
      Builder.setInsertionBlock(getDefaultBlock);
      auto *defaultValue = genExpression(paramDecl->init, paramName);
      auto *defaultResultBlock = Builder.getInsertionBlock();
      Builder.createBranchInst(storeBlock);

      // storeBlock:
      Builder.setInsertionBlock(storeBlock);
      auto *phi = Builder.createPhiInst(
          {param, defaultValue}, {currentBlock, defaultResultBlock});
      
    } else {
      auto *stackVar = Builder.createAllocStackInst(paramName);
      Builder.createStoreStackInst(param, stackVar);
    }
  }
}

void TreeIRGen::emitFunctionEpilogue(Value *returnValue) {
  Builder.createReturnInst(returnValue);
}
