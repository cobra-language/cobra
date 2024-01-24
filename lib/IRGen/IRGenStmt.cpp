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

void TreeIRGen::visitBlockStmt(BlockStmt *bs) {
  for (auto node : bs->body) {
    visit(node);
  }
}

void TreeIRGen::visitReturnStmt(ReturnStmt *rs) {
  
}

void TreeIRGen::visitIfStmt(IfStmt *is) {
  
}

void TreeIRGen::visitVariableStmt(VariableStmt *vs) {
  for (auto decl : vs->declarations) {
    visitDecl(decl);
  }
}

void TreeIRGen::visitExpressionStmt(ExpressionStmt *es) {
  
}

Instruction *TreeIRGen::emitLoad(Value *from) {
  
}

Instruction *TreeIRGen::emitStore(Value *storedValue, Value *ptr, bool declInit) {
  
}

Value *TreeIRGen::genExpression(ASTNode *expr, Identifier name) {
  if (auto *Iden = dynamic_cast<IdentifierExpr *>(expr)) {
    
  }
  
  if (auto *Lit = dynamic_cast<StringLiteralExpr *>(expr)) {
    return Builder.getLiteralString(*Lit->value);
  }
  
  if (auto *Lit = dynamic_cast<BooleanLiteralExpr *>(expr)) {
    return Builder.getLiteralBool(Lit->value);
  }
  
  if (auto *Lit = dynamic_cast<NumericLiteralExpr *>(expr)) {
    return Builder.getLiteralNumber(Lit->value);
  }
  
  return Builder.getLiteralUndefined();
}
