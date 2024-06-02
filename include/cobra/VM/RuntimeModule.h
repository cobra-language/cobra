/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RuntimeModule_h
#define RuntimeModule_h

#include <string>

#include "cobra/VM/Interpreter.h"
#include "cobra/BCGen/BytecodeRawData.h"

namespace cobra {
namespace vm {

class CodeBlock;
class Runtime;

class RuntimeModule {
  
  Runtime &runtime_;
  
  std::shared_ptr<BytecodeRawData> bytecode{};
  
public:
  explicit RuntimeModule(Runtime &runtime) : runtime_(runtime) {}
                         
  ~RuntimeModule();
                         
};

}
}

#endif /* Runtime_h */
