/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRGen_h
#define IRGen_h

#include <stdio.h>
#include <map>
#include <unordered_map>

#include "cobra/AST/ASTVisitor.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Scope.h"
#include "cobra/AST/Tree.h"
#include "cobra/Support/StringTable.h"

namespace cobra {
namespace Lowering {

using NameTableTy = std::map<Identifier, Value *>;
using NameTableScopeTy = std::map<Identifier, Value *>;

inline Identifier getNameFieldFromID(ASTNode *ID) {
  return Identifier::getFromPointer(dynamic_cast<IdentifierExpr *>(ID)->name);
}

class TreeIRGen : public ASTVisitor<TreeIRGen, Value *, Value *, Value *> {
  TreeIRGen(const TreeIRGen &) = delete;
  void operator=(const TreeIRGen &) = delete;
  
  Module *Mod;
  IRBuilder Builder;
  ASTNode *Root;
  Function *curFunction{};
  
  Scope *currentScope{};
  std::vector<Scope *> ScopeStack;
  
  NameTableTy nameTable{};
  
public:
  explicit TreeIRGen(ASTNode *root, Module *M);
  
  ~TreeIRGen();
  
  void visitChildren();
  void visitProgram(Program *fd) {};
  
  Value *visitFuncDecl(FuncDecl *fd);
  
  Value *visitParamDecl(ParamDecl *pd);
  
  Value *visitVariableDecl(VariableDecl *vd);
  
  Value *visitBlockStmt(BlockStmt *bs);
  
  Value *visitReturnStmt(ReturnStmt *rs);
  
  Value *visitIfStmt(IfStmt *is);
  
  Value *visitVariableStmt(VariableStmt *vs);
  
  Value *visitExpressionStmt(ExpressionStmt *es);
  
  Value *visitBooleanLiteralExpr(BooleanLiteralExpr *be);
  
  Value *visitNumericLiteralExpr(NumericLiteralExpr *ne);
  
  Value *visitStringLiteralExpr(StringLiteralExpr *se);
  
  Value *visitCallExpr(CallExpr *ce);
  
  Value *visitMemberExpr(MemberExpr *me);
  
  Value *visitIdentifierExpr(IdentifierExpr *ie);
  
  Value *visitUnaryExpr(UnaryExpr *ue);
  
  Value *visitPostfixUnaryExpr(PostfixUnaryExpr *pe);
  
  Value *visitBinaryExpr(BinaryExpr *be);
  
  Value *visitConditionalExpr(ConditionalExpr *ce);
  
  Value *visitSpreadElementExpr(SpreadElementExpr *se);
  
  Value *visitAssignmentExpr(AssignmentExpr *ae);
  
  void emitFunction(FuncDecl *fd);
  
  void emitFunctionPreamble(BasicBlock *entry);
  
  void emitParameters(AbstractFunctionDecl *funcNode);
  
  void emitFunctionEpilogue(Value *returnValue);
    
  void emitStatement(Stmt *stmt);
  
  Instruction *emitLoad(Value *from);
  
  Instruction *emitStore(Value *storedValue, Value *ptr, bool declInit);
      
  void emitExpressionBranch(
      Expr *expr,
      BasicBlock *onTrue,
      BasicBlock *onFalse,
      BasicBlock *onNullish);
  
  Value *ensureVariableExists(IdentifierExpr *id);
  
  
      
};

}
}

#endif /* IRGen_h */
