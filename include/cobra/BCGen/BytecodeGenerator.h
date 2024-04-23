/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BytecodeGenerator_h
#define BytecodeGenerator_h

#include <vector>

#include "cobra/BCGen/Bytecode.h"
#include "cobra/BCGen/RegAlloc.h"
#include "cobra/BCGen/BytecodeInstructionGenerator.h"

namespace cobra {

class BytecodeFunctionGenerator {
  
  std::vector<opcode_t> opcodes_{};
  
public:
  static std::unique_ptr<BytecodeFunctionGenerator> create() {
    return std::unique_ptr<BytecodeFunctionGenerator>(
        new BytecodeFunctionGenerator());
  }
  
  std::unique_ptr<BytecodeFunction> generateBytecodeFunction();
  
};

class BytecodeGenerator : public BytecodeInstructionGenerator {
  
  void emitMov(param_t dest, param_t src);
  
public:
  
  std::unique_ptr<BytecodeModule> generate();
  
};

}

#endif
