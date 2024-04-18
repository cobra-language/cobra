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

namespace cobra {
namespace vm {

class Runtime {
  
public:
 static std::shared_ptr<Runtime> create();

 ~Runtime();
  
  bool runBytecode();
  
};

}
}

#endif /* Runtime_h */
