/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

void TreeIRGen::emitStatement(Stmt *stmt) {
  visitStmt(stmt);
}

Value *TreeIRGen::visitBlockStmt(BlockStmt *bs) {
  for (auto node : bs->body) {
    visit(node);
  }
}

Value *TreeIRGen::visitReturnStmt(ReturnStmt *rs) {
  
}

Value *TreeIRGen::visitIfStmt(IfStmt *is) {
  
}

Value *TreeIRGen::visitVariableStmt(VariableStmt *vs) {
  for (auto decl : vs->declarations) {
    visitDecl(decl);
  }
}

Value *TreeIRGen::visitExpressionStmt(ExpressionStmt *es) {
  
}

Instruction *TreeIRGen::emitLoad(Value *from) {
  
}

Instruction *TreeIRGen::emitStore(Value *storedValue, Value *ptr, bool declInit) {
  
}

