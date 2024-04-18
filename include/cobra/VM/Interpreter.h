/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Interpreter_h
#define Interpreter_h

#include <cstdint>

#include "cobra/VM/CodeBlock.h"

namespace cobra {
namespace vm {

class Interpreter {
  
public:
  static bool interpretFunction(CodeBlock *codeBlock);
};

}
}

#endif /* Interpreter_h */
