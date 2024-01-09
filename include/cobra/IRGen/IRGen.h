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

class IRGenModul : public ASTVisitor {
  IRGenModul(const IRGenModul &) = delete;
  void operator=(const IRGenModul &) = delete;
  
  Module *Mod;
  IRBuilder Builder;
  Node *Root;
  
public:
  explicit IRGenModul(Node *root, Module *M);
  
  void visit();
  void visit(FunctionDeclarationNode *fd);
  void visit(VariableDeclaratorNode *vd);
  
  
  void emitFunction(FunctionDeclarationNode *fd);
  
};

}
}

#endif /* IRGen_hpp */
