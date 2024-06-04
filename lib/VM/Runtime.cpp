/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Runtime.h"
#include "cobra/VM/GCPointer.h"

using namespace cobra;
using namespace vm;

Runtime::~Runtime() {
  
}

std::shared_ptr<Runtime> Runtime::create() {
  return std::shared_ptr<Runtime>{new Runtime()};
}

bool Runtime::runBytecode(std::shared_ptr<BytecodeRawData> &&bytecode) {
  auto code = new CodeBlock(nullptr, 0, 0);
  return Interpreter::interpretFunction(code);
}
