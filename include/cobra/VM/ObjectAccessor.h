/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ObjectAccessor_h
#define ObjectAccessor_h

#include <cstddef>

namespace cobra {
namespace vm {

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
  
  
};

}
}

#endif /* ObjectAccessor_h */
