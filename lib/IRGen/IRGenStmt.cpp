/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IRGen/IRGen.h"

using namespace cobra;
using namespace Lowering;

void TreeIRGen::emitfunctionBody(Node *stmt) {
  if (auto *BS = dynamic_cast<BlockStatementNode *>(stmt)) {
    for (auto Node : BS->body_) {
      emitStatement(Node, false);
    }

    return;
  }
}

void TreeIRGen::emitStatement(Node *stmt, bool isLoopBody) {
  
}
