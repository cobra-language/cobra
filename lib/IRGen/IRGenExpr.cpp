/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

Value *TreeIRGen::visitBooleanLiteralExpr(BooleanLiteralExpr *be) {
  return Builder.getLiteralBool(be->value);
}

Value *TreeIRGen::visitNumericLiteralExpr(NumericLiteralExpr *ne) {
  return Builder.getLiteralNumber(ne->value);
}

Value *TreeIRGen::visitStringLiteralExpr(StringLiteralExpr *se) {
  return Builder.getLiteralString(*se->value);
}

Value *TreeIRGen::visitCallExpr(CallExpr *ce) {
  
}

Value *TreeIRGen::visitMemberExpr(MemberExpr *me) {
  
}

Value *TreeIRGen::visitIdentifierExpr(IdentifierExpr *ie) {
  
}

Value *TreeIRGen::visitUnaryExpr(UnaryExpr *ue) {
  
}

Value *TreeIRGen::visitPostfixUnaryExpr(PostfixUnaryExpr *pe) {
  
}

Value *TreeIRGen::visitBinaryExpr(BinaryExpr *be) {
  printf("visitBinaryExpr");
}

Value *TreeIRGen::visitSpreadElementExpr(SpreadElementExpr *se) {
  
}


Value *TreeIRGen::genBinaryExpression(BinaryExpr *bin) {
  
  
}
