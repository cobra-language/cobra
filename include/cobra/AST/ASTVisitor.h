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

template<typename ImplClass,
         typename... Args>
class ASTVisitor {
public:
  
  void visit(ASTNode *n) {
    if (auto *D = dynamic_cast<Decl *>(n)) {
      visitDecl(D);
    } else if (auto *S = dynamic_cast<Stmt *>(n)) {
      visitStmt(S);
    } else if (auto *E = dynamic_cast<Expr *>(n)) {
      visitExpr(E);
    }
  }
  
  void visitDecl(Decl *D, Args... AA) {
    switch (D->getKind()) {
      
#define DECL(CLASS, PARENT) \
    case DeclKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Decl(static_cast<CLASS##Decl*>(D), \
                             ::std::forward<Args>(AA)...);
#include "cobra/AST/DeclNodes.def"

    }
  }
  
  void visitExpr(Expr *E, Args... AA) {
    switch (E->getKind()) {

#define EXPR(CLASS, PARENT) \
    case ExprKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Expr(static_cast<CLASS##Expr*>(E), \
                             ::std::forward<Args>(AA)...);
#include "cobra/AST/ExprNodes.def"

    }
  }
  
  void visitStmt(Stmt *S, Args... AA) {
    switch (S->getKind()) {

#define STMT(CLASS, PARENT) \
    case StmtKind::CLASS: \
      return static_cast<ImplClass*>(this) \
        ->visit##CLASS##Stmt(static_cast<CLASS##Stmt*>(S), \
                             ::std::forward<Args>(AA)...);
#include "cobra/AST/StmtNodes.def"
    }
  }
  
};

}

#endif /* ASTVisitor_hpp */
