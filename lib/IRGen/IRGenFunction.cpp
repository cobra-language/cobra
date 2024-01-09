/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

void TreeIRGen::emitFunction(FunctionDeclarationNode *fd) {
  auto *body = getBlockStatement(fd);
  
  auto id = dynamic_cast<IdentifierNode *>(fd->id_);
  Function *newFunction = Builder.createFunction(id->name_);
  this->curFunction = newFunction;
  
  emitFunctionPreamble(Builder.createBasicBlock(newFunction));
  
  emitParameters(fd);
  
  emitfunctionBody(body);
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

void TreeIRGen::emitParameters(FunctionLikeNode *funcNode) {
  
}
