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

Runtime *Runtime::instance_ = nullptr;

Runtime::Runtime() {
  
}

Runtime::~Runtime() {
  
}

bool Runtime::create(const RuntimeOptions &options) {
  if (instance_ != nullptr) {
    return false;
  }
  instance_ = new Runtime;
  if (!instance_->init(std::move(options))) {
    instance_ = nullptr;
    return false;
  }
  return true;
}

bool Runtime::destroy() {
  if (instance_ == nullptr) {
    return false;
  }
  
  return true;
}

Runtime *Runtime::getCurrent() {
  return instance_;
}

inline HandleScope *Runtime::getTopScope() {
  return topScope_;
}

bool Runtime::runBytecode(std::shared_ptr<BytecodeRawData> &&bytecode) {
  auto code = new CodeBlock(nullptr, 0, 0);
  return Interpreter::interpretFunction(code);
}

bool Runtime::init(const RuntimeOptions &options) {
  
}
