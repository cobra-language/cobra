/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRGen_hpp
#define IRGen_hpp

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

class TreeIRGen : public ASTVisitor<TreeIRGen> {
  TreeIRGen(const TreeIRGen &) = delete;
  void operator=(const TreeIRGen &) = delete;
  
  Module *Mod;
  IRBuilder Builder;
  ASTNode *Root;
  Function *curFunction{};
  
  Scope *currentScope{};
  std::vector<Scope *> ScopeStack;
  
  NameTableTy nameTable_{};
  
public:
  explicit TreeIRGen(ASTNode *root, Module *M);
  
  ~TreeIRGen();
  
  void visit();
  void visitProgram(Program *fd) {};
  
  void visit(ASTNode *n);
  
  void visitFuncDecl(FuncDecl *fd);
  
  void visitParamDecl(ParamDecl *pd);
  
  void visitVariableDecl(VariableDecl *vd);
  
  void visitBlockStmt(BlockStmt *bs);
  
  void visitReturnStmt(ReturnStmt *rs);
  
  void visitIfStmt(IfStmt *is);
  
  void visitVariableStmt(VariableStmt *vs);
  
  void visitExpressionStmt(ExpressionStmt *es);
  
  void visitBooleanLiteralExpr(BooleanLiteralExpr *be);
  
  void visitNumericLiteralExpr(NumericLiteralExpr *ne);
  
  void visitStringLiteralExpr(StringLiteralExpr *se);
  
  void visitCallExpr(CallExpr *ce);
  
  void visitMemberExpr(MemberExpr *me);
  
  void visitIdentifierExpr(IdentifierExpr *ie);
  
  void visitUnaryExpr(UnaryExpr *ue);
  
  void visitPostfixUnaryExpr(PostfixUnaryExpr *pe);
  
  void visitBinaryExpr(BinaryExpr *be);
  
  void visitSpreadElementExpr(SpreadElementExpr *se);
  
  
  void emitFunction(FuncDecl *fd);
  
  void emitFunctionPreamble(BasicBlock *entry);
  
  void emitParameters(AbstractFunctionDecl *funcNode);
  
  void emitFunctionEpilogue(Value *returnValue);
    
  void emitStatement(Stmt *stmt);
  
  Instruction *emitLoad(Value *from);
  
  Instruction *emitStore(Value *storedValue, Value *ptr, bool declInit);
  
  Value *genExpression(ASTNode *expr, Identifier name = Identifier{});
  
  
      
};

}
}

#endif /* IRGen_hpp */
