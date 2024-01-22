/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ASTVisitor_hpp
#define ASTVisitor_hpp

#include "cobra/AST/Tree.h"

namespace cobra {

class ASTVisitor {
public:
  ASTVisitor() = default;
  virtual ~ASTVisitor() = default;
  
  virtual void visit(FuncDecl *fd) = 0;
  virtual void visit(VariableDecl *vd) = 0;
  
  
};

}

#endif /* ASTVisitor_hpp */
