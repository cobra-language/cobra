/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Bytecode_h
#define Bytecode_h

#include <vector>

namespace cobra {

using opcode_t = uint8_t;

struct FunctionHeader {
  uint32_t size;
  uint32_t paramCount;
  uint32_t functionNameID;
};

// This class represents the in-memory representation of the bytecode function.
class BytecodeFunction {
  
  const std::vector<opcode_t> opcodesAndJumpTables_;
  
};

class BytecodeModule {
  using FunctionList = std::vector<std::unique_ptr<BytecodeFunction>>;
  
  FunctionList functions_{};
  
public:
  explicit BytecodeModule() = default;
  
  uint32_t getNumFunctions() const {
    return functions_.size();
  }
  
  /// Add a new bytecode function to the module at \p index.
  void setFunction(uint32_t index, std::unique_ptr<BytecodeFunction> F);

  BytecodeFunction &getFunction(unsigned index);
  
};

}

#endif
