/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRBuilder_hpp
#define IRBuilder_hpp

#include <stdio.h>
#include "cobra/IR/IR.h"

namespace cobra {
namespace Lowering {

class IRBuilder {
  IRBuilder(const IRBuilder &) = delete;
  void operator=(const IRBuilder &) = delete;
  
  Module *M;
  BasicBlock *BB{};
  
public:
  explicit IRBuilder(Module *Mod) : M(Mod) {}
  
};

}
}

#endif /* IRBuilder_hpp */
