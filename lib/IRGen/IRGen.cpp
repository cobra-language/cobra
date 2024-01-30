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

void TreeIRGen::visitChildren() {
  Program *Program = dynamic_cast<class Program *>(Root);
  for (auto Node : Program->body) {
    visit(Node);
  }
}

Value *TreeIRGen::visitFuncDecl(FuncDecl *fd) { emitFunction(fd); }

Value *TreeIRGen::visitParamDecl(ParamDecl *pd) {
  
}

Value *TreeIRGen::visitVariableDecl(VariableDecl *vd) {
  Identifier name{};
  if (dynamic_cast<IdentifierExpr *>(vd->id))
    name = getNameFieldFromID(vd->id);
  std::cout << "VariableDecl: " << name.c_str() << std::endl;
//    Variable *V = Builder.createVariable(Variable::DeclKind::Var, name);
//    currentScope->insert(&name, V);
  auto *stackVar = Builder.createAllocStackInst(name);
  currentScope->insert(&name, stackVar);
  if (vd->init) {
    auto *storedValue = visitExpr(vd->init);
    Builder.createStoreStackInst(storedValue, stackVar);
  }
}
