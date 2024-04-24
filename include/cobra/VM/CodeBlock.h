/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CodeBlock_h
#define CodeBlock_h

#include <stdio.h>
#include <vector>

namespace cobra {
namespace vm {

class CodeBlock {
public:
  CodeBlock(
      const uint8_t *bytecode,
      const uint32_t bytecodeSize,
      uint32_t functionID)
      : bytecode_(bytecode),
        bytecodeSize_(bytecodeSize),
        functionID_(functionID) {
    
  }
  
  /// Pointer to the bytecode opcodes.
  const uint8_t *bytecode_;
  
  const uint32_t bytecodeSize_;
  
  /// ID of this function in the module's function list.
  uint32_t functionID_;
  
  

};

}
}

#endif /* CodeBlock_h */
