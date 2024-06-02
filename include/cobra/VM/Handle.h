/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef VM_h
#define VM_h

#include <stdint.h>

namespace cobra {
namespace vm {

using Address = uintptr_t;

class HandleBase {
  
  Address* handle_;
  
protected:
  
  const Address *handleRef() const {
    return handle_;
  }
  
public:
  explicit HandleBase(const Address *valueAddr)
      : handle_(const_cast<Address *>(valueAddr)) {
  }
  
  ~HandleBase() = default;
  
  bool isNull() const { return handle_ == nullptr; }
  
};

template <typename T>
class Handle final : public HandleBase {
  
public:
  explicit Handle(const Address *valueAddr, bool)
      : HandleBase(const_cast<Address *>(valueAddr)) {}
  
  Handle(const Handle<T>& handle) = default;
  Handle(Handle<T> &&) = default;
  
  
  
};

template<class T>
class MutableHandle : public Handle<T> {
public:
  
};

class HandleScope {
  
};

}
}

#endif /* VM_h */
