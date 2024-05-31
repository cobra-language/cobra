/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BytecodeGenerator_h
#define BytecodeGenerator_h

#include <vector>

#include "cobra/IR/IR.h"
#include "cobra/IR/Instrs.h"
#include "cobra/BCGen/Bytecode.h"
#include "cobra/BCGen/RegAlloc.h"
#include "cobra/BCGen/BytecodeInstructionGenerator.h"

namespace cobra {

class BytecodeGenerator;

struct Relocation {
  enum RelocationType {
    // A short jump instruction
    JumpType = 0,
    // A long jump instruction
    LongJumpType,
    // A basic block
    BasicBlockType,
    // A catch instruction
    CatchType,
  };
  
  /// The current location of this relocation.
  offset_t loc;
  /// Type of the relocation.
  RelocationType type;
  /// We multiplex pointer for different things under different types:
  /// If the type is jump or long jump, pointer is the target basic block;
  /// if the type is basic block, pointer is the pointer to it.
  /// if the type is catch instruction, pointer is the pointer to it.
  Value *pointer;
};

class BytecodeFunctionGenerator : public BytecodeInstructionGenerator {
  
  BytecodeGenerator &BCGen_;
  
  Function *F_;
  
  VirtualRegisterAllocator &RA_;
  
  /// For each Basic Block, we map to its beginning instruction location
  /// and the next basic block. We need this information to resolve jump
  /// targets and exception handler table.
  std::map<BasicBlock *, std::pair<offset_t, BasicBlock *>> basicBlockMap_{};
  
  /// The list of all jump instructions and jump targets that require
  /// relocation and address resolution.
  std::vector<Relocation> relocations_{};
      
  void emitMovIfNeeded(param_t dest, param_t src);
  
public:
  explicit BytecodeFunctionGenerator(
      BytecodeGenerator &BCGen,
      Function *F,
      VirtualRegisterAllocator &RA)
      : BCGen_(BCGen), F_(F), RA_(RA) {}
  
  static std::unique_ptr<BytecodeFunctionGenerator> create(
      BytecodeGenerator &BCGen,
      Function *F,
      VirtualRegisterAllocator &RA) {
    return std::unique_ptr<BytecodeFunctionGenerator>(
        new BytecodeFunctionGenerator(BCGen, F, RA));
  }
  
  /// Add long jump instruction to the relocation list.
  void addJumpToRelocations(offset_t loc, BasicBlock *target);
  
  void generateJumpTable();
  
  void resolveRelocations();
  
  unsigned encodeValue(Value *value);
  
#define INCLUDE_HBC_INSTRS
#define DEF_VALUE(CLASS, PARENT) \
  void generate##CLASS(CLASS *Inst, BasicBlock *next);
#include "cobra/IR/ValueKinds.def"
#undef DEF_VALUE
#undef MARK_VALUE
#undef INCLUDE_HBC_INSTRS
  
  void generateBody();
  
  void generateCodeBlock(BasicBlock *BB, BasicBlock *next);
  
  void generateInst(Instruction *ii, BasicBlock *next);
  
  std::unique_ptr<BytecodeFunction> generateBytecodeFunction();
  
  void shrinkJump(offset_t loc);
  
  void updateJumpTarget(offset_t loc, int newVal, int bytes);
  
  /// Change the opcode of a long jump instruction into a short jump.
  inline void longToShortJump(offset_t loc) {
    switch (opcodes_[loc]) {
#define DEFINE_JUMP_LONG_VARIANT(shortName, longName) \
  case longName##Op:                                  \
    opcodes_[loc] = shortName##Op;                    \
    break;
#include "cobra/BCGen/BytecodeList.def"
      default:
        COBRA_UNREACHABLE();
    }
  }
  
};

class BytecodeGenerator : public BytecodeInstructionGenerator {
  
  std::map<Function *, std::unique_ptr<BytecodeFunctionGenerator>>
      functionGenerators_{};
  std::vector<Function *> functions;
  std::map<Function *, unsigned> functionIDMap;
  
public:
  
  unsigned addFunction(Function *F);
  
  void setFunctionGenerator(
      Function *F,
      std::unique_ptr<BytecodeFunctionGenerator> BFG);
  
  std::unique_ptr<BytecodeModule> generate();
  
};

}

#endif
