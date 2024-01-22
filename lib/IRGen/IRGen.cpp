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
  
}

TreeIRGen::~TreeIRGen() {
  
}

void TreeIRGen::visit() {
  ProgramNode *Program = dynamic_cast<ProgramNode *>(Root);
  
  for (auto Node : Program->body) {
    switch (Node->getKind()) {
      case NodeKind::FuncDecl:
        auto fd = dynamic_cast<FuncDecl *>(Node);
        visit(fd);
        break;
    }
  }
}

void TreeIRGen::visit(FuncDecl *fd) { emitFunction(fd); }

void TreeIRGen::visit(VariableDecl *vd) {
  
}
