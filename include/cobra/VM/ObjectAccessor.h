/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ObjectAccessor_h
#define ObjectAccessor_h

#include <cstddef>
#include <atomic>

namespace cobra {
namespace vm {

class Object;

class ObjectAccessor {
public:
  template <class T, bool IsVolatile = false>
  static T getPrimitive(const void *obj, size_t offset) {
    return get<T, IsVolatile>(obj, offset);
  }

  template <class T, bool IsVolatile = false>
  static void setPrimitive(void *obj, size_t offset, T value) {
    set<T, IsVolatile>(obj, offset, value);
  }
  
  template <bool IsVolatile = false, bool needReadBarrier = true>
  static Object *getObject(const void *obj, size_t offset);

  template <bool IsVolatile = false, bool needWriterBarrier = true>
  static void setObject(void *obj, size_t offset, Object *value);
  
  template <class T>
  static void setFieldPrimitive(void *obj, size_t offset, T value, std::memory_order memory_order) {
    set<T>(obj, offset, value, memory_order);
  }

  template <bool needReadBarrier = true>
  static Object *getFieldObject(const void *obj, int offset, std::memory_order memory_order)  {
    return get<Object *>(obj, offset, memory_order);
  }
  
private:
  template<class T, bool IsVolatile = false>
  static T get(const void *obj, size_t offset) {
    auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(obj) + offset);
    if (IsVolatile) {
        // Atomic with seq_cst order reason: required for volatile
        return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_seq_cst);
    }
    // Atomic with relaxed order reason: to be compatible with other vms
    return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_relaxed);
  }
  
  template <class T, bool IsVolatile>
  static void set(void *obj, size_t offset, T value) {
    auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(obj) + offset);
    if (IsVolatile) {
        // Atomic with seq_cst order reason: required for volatile
        return reinterpret_cast<std::atomic<T> *>(addr)->store(value, std::memory_order_seq_cst);
    }
    // Atomic with relaxed order reason: to be compatible with other vms
    return reinterpret_cast<std::atomic<T> *>(addr)->store(value, std::memory_order_relaxed);
  }
  
  template <class T>
  static T get(const void *obj, size_t offset, std::memory_order memory_order) {
    auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(obj) + offset);
    // Atomic with parameterized order reason: memory order passed as argument
    return reinterpret_cast<const std::atomic<T> *>(addr)->load(memory_order);
  }

  template <class T>
  static void set(void *obj, size_t offset, T value, std::memory_order memory_order){
   auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(obj) + offset);
   // Atomic with parameterized order reason: memory order passed as argument
   return reinterpret_cast<std::atomic<T> *>(addr)->store(value, memory_order);
  }
  
  
};

}
}

#endif /* ObjectAccessor_h */
