/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

IRGenModul::IRGenModul(Tree::Node *root, Module *M) : Mod(M), Builder(Mod), Root(root) {
  
}

void IRGenModul::visit() {
  
}

void IRGenModul::visit(Tree::FunctionDeclarationNode *fd) { emitFunction(fd); }

void IRGenModul::visit(Tree::VariableDeclaratorNode *vd) {
  
}

void IRGenModul::emitFunction(Tree::FunctionDeclarationNode *fd) {
  
}
