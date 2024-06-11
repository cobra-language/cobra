/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Runtime_h
#define Runtime_h

#include <string>

#include "cobra/VM/Interpreter.h"
#include "cobra/BCGen/BytecodeRawData.h"
#include "cobra/VM/Handle.h"

namespace cobra {
namespace vm {

class Runtime {
  
  HandleScope *topScope_{};
  
public:
 static std::shared_ptr<Runtime> create();

 ~Runtime();
  
  HandleScope *getTopScope();
  
  bool runBytecode(std::shared_ptr<BytecodeRawData> &&bytecode);
  
};

}
}

#endif /* Runtime_h */
