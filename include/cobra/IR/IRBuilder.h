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

class IRBuilder {
  IRBuilder(const IRBuilder &) = delete;
  void operator=(const IRBuilder &) = delete;
  
  Module *M;
  BasicBlock::iterator InsertionPoint{};
  BasicBlock *Block{};
  
public:
  explicit IRBuilder(Module *Mod) : M(Mod) {}
  
  Module *getModule() {
    return M;
  }
  
  BasicBlock *createBasicBlock(Function *Parent);
  
  Function *createFunction(std::string Name);
  
  void setInsertionBlock(BasicBlock *BB);
  
  BasicBlock *getInsertionBlock();
  
  void setInsertionPoint(Instruction *IP);
  
};

}

#endif /* IRBuilder_hpp */
