/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRGen_hpp
#define IRGen_hpp

#include <stdio.h>

#include "cobra/AST/ASTVisitor.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/AST/Tree.h"

namespace cobra {
namespace Lowering {

class TreeIRGen : public ASTVisitor {
  TreeIRGen(const TreeIRGen &) = delete;
  void operator=(const TreeIRGen &) = delete;
  
  Module *Mod;
  IRBuilder Builder;
  ASTNode *Root;
  Function *curFunction{};
  
public:
  explicit TreeIRGen(ASTNode *root, Module *M);
  
  void visit();
  void visit(FunctionDeclarationNode *fd);
  void visit(VariableDeclaratorNode *vd);
  
  
  void emitFunction(FunctionDeclarationNode *fd);
  
  void emitFunctionPreamble(BasicBlock *entry);
  
  void emitParameters(FunctionLikeNode *funcNode);
  
  void emitfunctionBody(ASTNode *stmt);
  
  void emitStatement(ASTNode *stmt, bool isLoopBody);
  
  NodeList getParams(FunctionLikeNode *node);
  
  BlockStatementNode *getBlockStatement(FunctionLikeNode *node);
  
};

}
}

#endif /* IRGen_hpp */
