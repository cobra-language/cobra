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
  using ScopeListTy = std::vector<Scope *>;
  
  Scope *createInnerScope() {
    auto *S = new Scope(this);
    innerScopes.emplace_back(S);
    return S;
  }
  
  Scope *getParent() const {
    return parent;
  }
  
  ScopeListTy &getMutableInnerScopes() {
    return innerScopes;
  }

  const ScopeListTy &getInnerScopes() const {
    return innerScopes;
  }
  
  VariableListType &getMutableVariables() {
    return variables;
  }

  const VariableListType &getVariables() const {
    return variables;
  }
  
  void addVariable(Variable *V) {
    variables.emplace_back(V);
  }
  
private:
  Scope *parent{};
  ScopeListTy innerScopes;
  
  VariableListType variables;
  
};

}
#endif /* Scope_hpp */
