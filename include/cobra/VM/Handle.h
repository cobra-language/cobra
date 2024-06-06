/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef VM_h
#define VM_h

#include <stdint.h>

#include "cobra/VM/CBValue.h"
#include "cobra/VM/GCPointer.h"
#include "cobra/VM/CBObject.h"

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
private:
  uint32_t size_ = 0;
  
  HandleScope* const link_;
  
  const int32_t capacity_;
  
  static constexpr size_t CapacityOffset(PointerSize pointer_size) {
    return static_cast<size_t>(pointer_size);
  }
  
  static constexpr size_t ReferencesOffset(PointerSize pointer_size) {
    return CapacityOffset(pointer_size) + sizeof(capacity_) + sizeof(size_);
  }
  
  StackReference<CBObject>* getReferences() const {
    uintptr_t address = reinterpret_cast<uintptr_t>(this) + ReferencesOffset(kRuntimePointerSize);
    return reinterpret_cast<StackReference<CBObject>*>(address);
  }
  
public:
  explicit HandleScope(HandleScope* link, uint32_t capacity) : link_(link), capacity_(capacity) {}
  
  ~HandleScope() {}
  
  ObjPtr<CBObject> getReference(size_t i) const;
  
  template<class T>
  Handle<T> getHandle(size_t i);
  
  template<class T>
  MutableHandle<T> getMutableHandle(size_t i);
  
  void setReference(size_t i, ObjPtr<CBObject> object);
  
  template<class T>
  Handle<T> makeHandle(T *object);
  
  template<class T>
  MutableHandle<T> makeHandle(ObjPtr<T> object);
  
  template<class T>
  MutableHandle<T> makeMutableHandle(T *object);
  
  template<class T>
  MutableHandle<T> makeMutableHandle(ObjPtr<T> object);
  
  inline uint32_t size() const {
    return size_;
  }
  
  uint32_t capacity() const {
    return static_cast<uint32_t>(capacity_);
  }
  
};

}
}

#endif /* VM_h */
