/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Runtime.h"

using namespace cobra;
using namespace vm;

Runtime::~Runtime() {
  
}

std::shared_ptr<Runtime> Runtime::create() {
  return std::shared_ptr<Runtime>{new Runtime()};
}

bool Runtime::runBytecode() {
  CodeBlock *code = new CodeBlock();
  return Interpreter::interpretFunction(code);
}
