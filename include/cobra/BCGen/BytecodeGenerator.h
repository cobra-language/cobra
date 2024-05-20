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

class BytecodeFunctionGenerator : public BytecodeInstructionGenerator {
    
  void emitMovIfNeeded(param_t dest, param_t src);
  
public:
  static std::unique_ptr<BytecodeFunctionGenerator> create() {
    return std::unique_ptr<BytecodeFunctionGenerator>(
        new BytecodeFunctionGenerator());
  }
  
  unsigned encodeValue(Value *value);
  
#define INCLUDE_HBC_INSTRS
#define DEF_VALUE(CLASS, PARENT) \
  void generate##CLASS(CLASS *Inst, BasicBlock *next);
#include "cobra/IR/ValueKinds.def"
#undef DEF_VALUE
#undef MARK_VALUE
#undef INCLUDE_HBC_INSTRS
  
  void generate(Instruction *ii, BasicBlock *next);
  
  std::unique_ptr<BytecodeFunction> generateBytecodeFunction();
  
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
