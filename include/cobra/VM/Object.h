/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Object_h
#define Object_h

#include <atomic>
#include "cobra/VM/CBValue.h"
#include "cobra/VM/GCCell.h"
#include "cobra/Support/ArraySlice.h"
#include "cobra/VM/GCPointer.h"
#include "cobra/VM/Modifiers.h"
#include "cobra/VM/ObjectAccessor.h"
#include "cobra/VM/RuntimeGlobals.h"
#include "cobra/Support/StringRef.h"
#include "cobra/Support/Common.h"

namespace cobra {
namespace vm {

class Class;

class Object : public GCCell {
  /// The Class representing the type of the object.
  GCPointer<Class> clazz{nullptr};
  
public:
  
  Object() = default;
  
  static constexpr uint32_t instanceSize() {
    return sizeof(Object);
  }
  
  static constexpr uint32_t classOffset() {
    return MEMBER_OFFSET(Object, clazz);
  }
  
  inline Class *getClass() {
    return clazz.get<Class>();
  }
  
  /// Ref to https://android.googlesource.com/platform/art/+/refs/heads/main/runtime/mirror/object.h#373
  /// and https://gitee.com/openharmony/arkcompiler_runtime_core/blob/master/static_core/runtime/include/object_accessor-inl.h#L111
  template<class T, bool IsVolatile = false>
  inline T getFieldPrimitive(size_t offset) {
    auto *addr = reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(this) + offset);
    if (IsVolatile) {
        // Atomic with seq_cst order reason: required for volatile
        return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_seq_cst);
    }
    // Atomic with relaxed order reason: to be compatible with other vms
    return reinterpret_cast<const std::atomic<T> *>(addr)->load(std::memory_order_relaxed);
  }
  
  template<class T>
  T getField(size_t offset) {
    return getFieldPrimitive<T>(offset);
  }
  
};

}
}

#endif /* Object_h */
