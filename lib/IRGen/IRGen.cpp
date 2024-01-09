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

TreeIRGen::TreeIRGen(Node *root, Module *M) : Mod(M), Builder(Mod), Root(root) {
  
}

void TreeIRGen::visit() {
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

void TreeIRGen::visit(FunctionDeclarationNode *fd) { emitFunction(fd); }

void TreeIRGen::visit(VariableDeclaratorNode *vd) {
  
}

BlockStatementNode *TreeIRGen::getBlockStatement(FunctionLikeNode *node) {
  switch (node->getKind()) {
    case NodeKind::FunctionDeclaration:
      return dynamic_cast<BlockStatementNode *>(dynamic_cast<FunctionDeclarationNode *>(node)->body_);
  }
}
