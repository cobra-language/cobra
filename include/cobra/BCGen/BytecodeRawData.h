/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef BytecodeRawData_h
#define BytecodeRawData_h

#include <vector>

#include "cobra/BCGen/Bytecode.h"

namespace cobra {

class BytecodeRawData {
  std::unique_ptr<BytecodeModule> byteCodeModule_;
  
public:
  explicit BytecodeRawData(std::unique_ptr<BytecodeModule> byteCodeModule);
  
  static std::unique_ptr<BytecodeRawData> create(
      std::unique_ptr<BytecodeModule> byteCodeModule) {
    return std::unique_ptr<BytecodeRawData>(
        new BytecodeRawData(std::move(byteCodeModule)));
  }
  
  
};

}

#endif
