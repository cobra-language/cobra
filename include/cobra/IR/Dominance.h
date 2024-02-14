/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Scope_h
#define Scope_h

#include <stdio.h>
#include <map>
#include "cobra/IR/IR.h"

namespace cobra {

class DominanceInfo {
public:
  explicit DominanceInfo(Function *F);
  
  ~DominanceInfo();
  
};

}
#endif /* Scope_h */
