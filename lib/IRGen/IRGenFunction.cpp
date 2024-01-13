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
  auto id = dynamic_cast<IdentifierNode *>(fd->id);
  Function *newFunction = Builder.createFunction(id->name);
  this->curFunction = newFunction;
  
  emitFunctionPreamble(Builder.createBasicBlock(newFunction));
  
  emitParameters(fd);
  
  emitfunctionBody(fd->body);
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
  uint32_t paramIndex = uint32_t{0} - 1;
  for (auto elem : funcNode->params) {
    ASTNode *init = nullptr;
    ++paramIndex;
    
    auto paramNameDecl = dynamic_cast<ParamDecl *>(elem)->id;
    auto paramName = dynamic_cast<IdentifierNode *>(paramNameDecl)->name;
    
    std::cout << paramName << std::endl;
    
    auto *Param = Builder.createParameter(this->curFunction , paramName);
    
    
  }
}
