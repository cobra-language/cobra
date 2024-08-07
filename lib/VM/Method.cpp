/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Method.h"
#include "cobra/VM/Runtime.h"

using namespace cobra;
using namespace vm;

class Interpreter;

void Method::invoke(uint32_t *args, uint32_t argCount) {
  auto frame = Runtime::getCurrent()->getCurrentFrame();
  StackFrame *newFrame = StackFrame::create(frame, this, argCount);
  if (!isNative()) {
    Interpreter::execute(newFrame);
  }
  
  invokeCompiledCode(args, argCount);
}

void Method::invokeCompiledCode(uint32_t *args, uint32_t argCount) {
  auto frame = Runtime::getCurrent()->getCurrentFrame();
  auto newFrame = StackFrame::create(frame, this, argCount);
}
