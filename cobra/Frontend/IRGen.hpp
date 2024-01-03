/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRGen_hpp
#define IRGen_hpp

#include <stdio.h>
#include "ASTVisitor.hpp"
#include "IR.hpp"
#include "IRBuilder.hpp"

namespace cobra {
namespace Lowering {

class IRGenModul : public Tree::ASTVisitor {
  IRGenModul(const IRGenModul &) = delete;
  void operator=(const IRGenModul &) = delete;
  
  Module *M;
  IRBuilder Builder;
  Tree::Node *Root;
  
public:
  explicit IRGenModul(Module *Mod) : M(Mod) {}
  
  void visit(Tree::Node *D);
  
  
};

}
}

#endif /* IRGen_hpp */
