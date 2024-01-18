/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Scope_hpp
#define Scope_hpp

#include <stdio.h>
#include <vector>
#include "cobra/IR/IR.h"

namespace cobra {

class Scope {
  using VariableListType = std::vector<Variable *>;
  
  Scope(const Scope &) = delete;
  Scope &operator=(const Scope &) = delete;
  
  explicit Scope(Scope *p) : parent(p) {}

  ~Scope();
  
public:
  
  Scope *getParent() const {
    return parent;
  }
  
  
private:
  Scope *parent{};
  
};

}
#endif /* Scope_hpp */
