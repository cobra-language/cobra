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

/// A Handle provides a reference to an object that survives relocation by
/// the garbage collector.
/// This is a very lightweight class: copies are very cheap (just copying a
/// register); construction is also relatively cheap (in the common case a
/// comparison and increment).
class HandleBase {
  
  Address location_;
  
public:
  explicit HandleBase(Address location) : location_(location) {}
  
  template <typename T>
  explicit HandleBase(T value);
  
  ~HandleBase() = default;
  
  bool isNull() const {
    return location_ == reinterpret_cast<uintptr_t>(location_);
  }
  
  Address address() const {
    return location_;
  }
};

template <typename T>
class Handle final : public HandleBase {
  
public:
  explicit Handle(Address *location) : HandleBase(location) {}
  
  Handle(const Handle<T>& handle) = default;
  Handle(Handle<T> &&) = default;
  
  T *get() const {
    
  }
  
  T operator*() const {
    return get();
  }
  
  T *operator->() const {
    
  }
  
};

template<class T>
class MutableHandle : public Handle<T> {
public:
  
};

class HandleScope {
private:
  uint32_t size_ = 0;
  
  HandleScope *const prev_;
  
  const int32_t capacity_;
  
  static constexpr size_t capacityOffset(PointerSize pointer_size) {
    return static_cast<size_t>(pointer_size);
  }
  
  static constexpr size_t referencesOffset(PointerSize pointer_size) {
    return capacityOffset(pointer_size) + sizeof(capacity_) + sizeof(size_);
  }
  
  StackReference<CBObject> *getReferences() const {
    uintptr_t address = reinterpret_cast<uintptr_t>(this) + referencesOffset(kRuntimePointerSize);
    return reinterpret_cast<StackReference<CBObject>*>(address);
  }
  
public:
  explicit HandleScope(HandleScope* prev, uint32_t capacity) : prev_(prev), capacity_(capacity) {}
  
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
