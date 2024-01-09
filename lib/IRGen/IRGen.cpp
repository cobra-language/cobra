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

IRGenModul::IRGenModul(Node *root, Module *M) : Mod(M), Builder(Mod), Root(root) {
  
}

void IRGenModul::visit() {
  ProgramNode *Program = dynamic_cast<ProgramNode *>(Root);
  
  for (auto Node : Program->body_) {
    switch (Node->getKind()) {
      case NodeKind::FunctionDeclaration:
        auto fd = dynamic_cast<FunctionDeclarationNode *>(Node);
        visit(fd);
        break;
    }
  }
}

void IRGenModul::visit(FunctionDeclarationNode *fd) { emitFunction(fd); }

void IRGenModul::visit(VariableDeclaratorNode *vd) {
  
}

void IRGenModul::emitFunction(FunctionDeclarationNode *fd) {
  auto id = dynamic_cast<IdentifierNode *>(fd->id_);
  auto name = id->name_;
}
