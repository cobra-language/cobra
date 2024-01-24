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

template<typename ImplClass>
class ASTVisitor {
public:
  
  void visitDecl(Decl *E) {
    switch (E->getKind()) {
      
#define DECL(CLASS, PARENT) \
    case DeclKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Decl(static_cast<CLASS##Decl*>(E));
#include "cobra/AST/DeclNodes.def"

    }
  }
  
  void visitExpr(Expr *E) {
    switch (E->getKind()) {

#define EXPR(CLASS, PARENT) \
    case ExprKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Expr(static_cast<CLASS##Expr*>(E));
#include "cobra/AST/ExprNodes.def"

    }
  }
  
  void visitStmt(Stmt *S) {
    switch (S->getKind()) {

#define STMT(CLASS, PARENT) \
    case StmtKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Stmt(static_cast<CLASS##Stmt*>(S));
#include "cobra/AST/StmtNodes.def"
    }
  }
  
};

}

#endif /* ASTVisitor_hpp */
