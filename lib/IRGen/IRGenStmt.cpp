/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

void TreeIRGen::emitfunctionBody(ASTNode *stmt) {
  if (auto *BS = dynamic_cast<BlockStmt *>(stmt)) {
    for (auto Node : BS->body) {
      emitStatement(Node, false);
    }

    return;
  }
}

void TreeIRGen::emitStatement(ASTNode *stmt, bool isLoopBody) {
  
}

Instruction *TreeIRGen::emitLoad(Value *from) {
  
}

Instruction *TreeIRGen::emitStore(Value *storedValue, Value *ptr, bool declInit) {
  
}

Value *TreeIRGen::genExpression(ASTNode *expr, Identifier name) {
  if (auto *Iden = dynamic_cast<IdentifierNode *>(expr)) {
    
  }
  
  if (auto *Lit = dynamic_cast<StringLiteralNode *>(expr)) {
    return Builder.getLiteralString(*Lit->value);
  }
  
  if (auto *Lit = dynamic_cast<BooleanLiteralNode *>(expr)) {
    return Builder.getLiteralBool(Lit->value);
  }
  
  if (auto *Lit = dynamic_cast<NumericLiteralNode *>(expr)) {
    return Builder.getLiteralNumber(Lit->value);
  }
  
  return Builder.getLiteralUndefined();
}
