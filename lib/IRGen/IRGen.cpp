/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"
#include <iostream>
#include <fstream>

using namespace cobra;
using namespace Lowering;

TreeIRGen::TreeIRGen(ASTNode *root, Module *M) : Mod(M), Builder(Mod), Root(root) {
  currentScope = new Scope(nullptr);
}

TreeIRGen::~TreeIRGen() {
  
}

void TreeIRGen::visit() {
  Program *Program = dynamic_cast<class Program *>(Root);
  for (auto Node : Program->body) {
    visit(Node);
  }
}

void TreeIRGen::visit(ASTNode *n) {
  if (auto *D = dynamic_cast<Decl *>(n)) {
    visitDecl(D);
  } else if (auto *S = dynamic_cast<Stmt *>(n)) {
    visitStmt(S);
  } else if (auto *E = dynamic_cast<Expr *>(n)) {
    visitExpr(E);
  }
}

void TreeIRGen::visitFuncDecl(FuncDecl *fd) { emitFunction(fd); }

void TreeIRGen::visitParamDecl(ParamDecl *pd) {
  
}

void TreeIRGen::visitVariableDecl(VariableDecl *vd) {  
  if (vd->init) {
    Identifier name{};
    if (dynamic_cast<IdentifierExpr *>(vd->id))
      name = getNameFieldFromID(vd->id);
    std::cout << "VariableDecl: " << name.c_str() << std::endl;
    Variable *V = Builder.createVariable(Variable::DeclKind::Var, name);
    currentScope->insert(&name, V);
    
    auto *stackVar = Builder.createAllocStackInst(name);
    Builder.createStoreStackInst(V, stackVar);
    
  } else {
    
  }
  
}
