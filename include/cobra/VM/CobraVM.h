/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CobraVM_h
#define CobraVM_h

#include "cobra/VM/Runtime.h"

namespace cobra {
namespace vm {

class CobraVM {
  
  static CobraVM *create(Runtime *runtime, const RuntimeOptions &options);
  
};

}
}

#endif /* CobraVM_h */
