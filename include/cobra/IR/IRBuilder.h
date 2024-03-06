/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef IRBuilder_h
#define IRBuilder_h

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
  
  LiteralEmpty literalEmpty{};
  LiteralUndefined literalUndefined{};
  LiteralNull literalNull{};
  LiteralBool literalFalse{false};
  LiteralBool literalTrue{true};
  
public:
  explicit IRBuilder(Module *Mod) : M(Mod) {}
  
  explicit IRBuilder(Function *F) : M(F->getParent()) {}
  
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
  
  Function *getFunction() {
    assert(Block && "Builder has no current function");
    return Block->getParent();
  }
  
  void setInsertionPoint(Instruction *IP);
  
  BranchInst *createBranchInst(BasicBlock *Destination);
  
  CondBranchInst *
  createCondBranchInst(Value *Cond, BasicBlock *T, BasicBlock *F);
  
  ReturnInst *createReturnInst(Value *Val);
  
  AllocStackInst *createAllocStackInst(Identifier varName);
  
  LoadStackInst *createLoadStackInst(AllocStackInst *ptr);
  
  StoreStackInst *createStoreStackInst(Value *storedValue, AllocStackInst *ptr);
  
  BinaryOperatorInst *createBinaryOperatorInst(
      Value *left,
      Value *right,
      BinaryOperatorInst::OpKind opKind);
  
  PhiInst *createPhiInst(
      const PhiInst::ValueListType &values,
      const PhiInst::BasicBlockListType &blocks);
  
  PhiInst *createPhiInst();
  
  /// This is an RAII object that destroys instructions when it is destroyed.
  class InstructionDestroyer {
    InstructionDestroyer(const InstructionDestroyer &) = delete;
    void operator=(const InstructionDestroyer &) = delete;

    std::vector<Instruction *> list{};

   public:
    explicit InstructionDestroyer() = default;

    /// \returns true if the instruction \p A is already in the destruction
    /// queue. Notice that this is an O(n) search and should only be used for
    /// debugging.
    bool hasInstruction(Instruction *A) {
      return std::find(list.begin(), list.end(), A) != list.end();
    }

    /// Add the instruction \p  A to the list of instructions to delete.
    void add(Instruction *A) {
#ifndef NDEBUG
      assert(!hasInstruction(A) && "Instruction already in list!");
#endif
      list.push_back(A);
    }

    ~InstructionDestroyer() {
      for (auto *I : list) {
        I->eraseFromParent();
      }
    }
  };
  
};

}

#endif /* IRBuilder_h */
