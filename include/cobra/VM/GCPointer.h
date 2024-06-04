/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GCPointer_h
#define GCPointer_h

#include <iosfwd>
#include <type_traits>
#include <cassert>
#include <cstddef>
#include <string.h>

namespace cobra {
namespace vm {

static constexpr size_t kObjectAlignmentShift = 3;

// ref art obj_ptr.h and hermes PointerBase
template<typename T>
class ObjPtr {
  
private:
  uintptr_t reference_;
  
  static uintptr_t encode(T *ptr);
  
public:
  explicit ObjPtr() : ObjPtr(nullptr) {}
  
  explicit ObjPtr(std::nullptr_t) : reference_(0u) {}
  
  template <typename Type,
            typename = typename std::enable_if_t<std::is_base_of_v<T, Type>>>
  ObjPtr(Type *ptr);

  template <typename Type,
            typename = typename std::enable_if_t<std::is_base_of_v<T, Type>>>
  ObjPtr(const ObjPtr<Type> &other);

  template <typename Type,
            typename = typename std::enable_if_t<std::is_base_of_v<T, Type>>>
  ObjPtr &operator=(const ObjPtr<Type> &other);
  
  ObjPtr &operator=(T *ptr);
  
  T *operator->() const;
  
  bool isNull() const {
    return reference_ == 0;
  }
  
  T *ptr() const {
    return reinterpret_cast<T*>(reference_);
  }
  
  /// Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<T> downCast(ObjPtr<SourceType> ptr);

  /// Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<T> downCast(SourceType *ptr);
};

template <typename T>
class GCPointer {
  
};

}
}

#endif /* GCPointer_h */
