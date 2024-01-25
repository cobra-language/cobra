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
  using ValueListType = std::map<Identifier *, Value *>;
  
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
  
  ValueListType &getMutableVariables() {
    return values;
  }

  const ValueListType &getVariables() const {
    return values;
  }
  
  void insert(Identifier *K, Value *V) {
    values.insert({K, V});
  }
  
  Value *lookup(Identifier *K) {
    auto it = values.find(K);
    return it == values.end() ? nullptr : it->second;
  }
  
private:
  Scope *parent{};
  ScopeListTy innerScopes;
  
  ValueListType values;
  
};

}
#endif /* Scope_hpp */
