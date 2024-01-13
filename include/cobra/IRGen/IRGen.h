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
  void visit(FuncDecl *fd);
  void visit(VariableDeclaratorNode *vd);
  
  
  void emitFunction(FuncDecl *fd);
  
  void emitFunctionPreamble(BasicBlock *entry);
  
  void emitParameters(AbstractFunctionDecl *funcNode);
  
  void emitfunctionBody(ASTNode *stmt);
  
  void emitStatement(ASTNode *stmt, bool isLoopBody);
    
  BlockStmt *getBlockStatement(AbstractFunctionDecl *node);
  
};

}
}

#endif /* IRGen_hpp */
