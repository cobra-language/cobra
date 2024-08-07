/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Interpreter_h
#define Interpreter_h

#include <cstdint>

#include "cobra/VM/Method.h"
#include "cobra/VM/StackFrame.h"
#include "cobra/VM/Runtime.h"

namespace cobra {
namespace vm {

// dalvik
struct InterpSaveState {
  
  
};

class Interpreter {
  
public:
  static bool execute(Method *method, uint32_t *args, uint32_t argCount);
  
  static bool execute(StackFrame *frame);
  
};

}
}

#endif /* Interpreter_h */
