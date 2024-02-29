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
  Value *Value;
  // Generate IR for the return value, or undefined if this is an empty return
  // statement.
  if (auto *A = rs->argument) {
    Value = visitExpr(A);
  } else {
    Value = Builder.getLiteralUndefined();
  }
  Builder.createReturnInst(Value);
  auto Parent = Builder.getInsertionBlock()->getParent();
  Builder.setInsertionBlock(Builder.createBasicBlock(Parent));
}

Value *TreeIRGen::visitIfStmt(IfStmt *is) {
  auto parent = Builder.getInsertionBlock()->getParent();
  auto thenBB = Builder.createBasicBlock(parent);
  auto elseBB = Builder.createBasicBlock(parent);
  auto continueBB = Builder.createBasicBlock(parent);
  
  emitExpressionBranch(is->Condition, thenBB, elseBB, nullptr);
  
  Builder.setInsertionBlock(thenBB);
  visit(is->Then);
  Builder.createBranchInst(continueBB);

  // IRGen the Else, if it exists:
  Builder.setInsertionBlock(elseBB);
  if (is->Else) {
    visit(is->Else);
  }

  Builder.createBranchInst(continueBB);
  Builder.setInsertionBlock(continueBB);
}

Value *TreeIRGen::visitVariableStmt(VariableStmt *vs) {
  for (auto decl : vs->declarations) {
    visitDecl(decl);
  }
}

Value *TreeIRGen::visitExpressionStmt(ExpressionStmt *es) {
  Value *value = visitExpr(es->expression);
}

Instruction *TreeIRGen::emitLoad(Value *from) {
  
}

Instruction *TreeIRGen::emitStore(Value *storedValue, Value *ptr, bool declInit) {
  
}

