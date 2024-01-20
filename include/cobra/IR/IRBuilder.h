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
#include "cobra/Support/StringTable.h"
#include "cobra/IR/Instrs.h"
#include "cobra/AST/Context.h"

namespace cobra {

class IRBuilder {
  IRBuilder(const IRBuilder &) = delete;
  void operator=(const IRBuilder &) = delete;
  
  Module *M;
  BasicBlock::iterator InsertionPoint{};
  BasicBlock *Block{};
  SMLoc Location{};
  
public:
  explicit IRBuilder(Module *Mod) : M(Mod) {}
  
  Module *getModule() {
    return M;
  }
  
  BasicBlock *createBasicBlock(Function *Parent);
  
  Function *createFunction(Identifier OriginalName);
  
  Parameter *createParameter(Function *Parent, Identifier OriginalName);
  
  Variable *createVariable(Variable::DeclKind declKind, Identifier Name);
  
  LiteralNumber *getLiteralNumber(double value);
  
  LiteralString *getLiteralString(StringRef value);
  
  LiteralBool *getLiteralBool(bool value);
  
  LiteralEmpty *getLiteralEmpty();
  
  LiteralUndefined *getLiteralUndefined();
  
  LiteralNull *getLiteralNull();
  
  Identifier createIdentifier(StringRef str);
  
private:
  void insert(Instruction *Inst);
  
public:
  
  void setInsertionBlock(BasicBlock *BB);
  
  BasicBlock *getInsertionBlock();
  
  void setInsertionPoint(Instruction *IP);
  
  ReturnInst *createReturnInst(Value *Val);
  
  AllocStackInst *createAllocStackInst(Identifier varName);
  
};

}

#endif /* IRBuilder_hpp */
