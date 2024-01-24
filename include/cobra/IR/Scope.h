/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Scope_hpp
#define Scope_hpp

#include <stdio.h>
#include <map>
#include "cobra/IR/IR.h"

namespace cobra {

class Scope {
  using VariableListType = std::map<Identifier *, Variable *>;
  
  Scope(const Scope &) = delete;
  Scope &operator=(const Scope &) = delete;
  
public:
  using ScopeListTy = std::vector<Scope *>;
  
  explicit Scope(Scope *p) : parent(p) {}
  
  ~Scope();
  
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
  
  void insert(Identifier *K, Variable *V) {
    variables.insert({K, V});
  }
  
  Variable *lookup(Identifier *K) {
    auto it = variables.find(K);
    return it == variables.end() ? nullptr : it->second;
  }
  
private:
  Scope *parent{};
  ScopeListTy innerScopes;
  
  VariableListType variables;
  
};

}
#endif /* Scope_hpp */
