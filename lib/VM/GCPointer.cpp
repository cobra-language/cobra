/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/GCPointer.h"

using namespace cobra;
using namespace vm;

template<class T>
inline uintptr_t ObjPtr<T>::encode(T *ptr) {
  uintptr_t ref = reinterpret_cast<uintptr_t>(ptr);
  return ref;
}

template<class T>
template <typename Type,
          typename /* = typename std::enable_if_t<std::is_base_of_v<T, Type>> */>
inline ObjPtr<T>::ObjPtr(Type *ptr)
    : reference_(encode(static_cast<T*>(ptr))) {
}

template<class T>
template <typename Type,
          typename /* = typename std::enable_if_t<std::is_base_of_v<T, Type>> */>
inline ObjPtr<T>::ObjPtr(const ObjPtr<Type> &other)
    : reference_(other.reference_) {}

template<class T>
template <typename Type,
          typename /* = typename std::enable_if_t<std::is_base_of_v<T, Type>> */>
inline ObjPtr<T> &ObjPtr<T>::operator=(const ObjPtr<Type> &other) {
  reference_ = other.reference_;
  return *this;
}

template<class T>
inline ObjPtr<T> &ObjPtr<T>::operator=(T *ptr) {
  reference_ = encode(ptr);
  return *this;
}

template<class T>
inline T *ObjPtr<T>::operator->() const {
  return ptr();
}

template<class T>
template <typename SourceType>
inline ObjPtr<T> ObjPtr<T>::downCast(ObjPtr<SourceType> ptr) {
  static_assert(std::is_base_of_v<SourceType, T>,
                "Target type must be a subtype of source type");
  return static_cast<T*>(ptr.ptr());
}

template<class T>
template <typename SourceType>
inline ObjPtr<T> ObjPtr<T>::downCast(SourceType *ptr) {
  static_assert(std::is_base_of_v<SourceType, T>,
                "Target type must be a subtype of source type");
  return static_cast<T*>(ptr);
}

template<class T>
inline uint32_t PtrCompression<T>::compress(ObjPtr<T> ptr) {
  return compress(ptr.ptr());
}

template<class T>
inline void ObjectReference<T>::assign(ObjPtr<T> ptr) {
  assign(ptr.ptr());
}

template <typename T>
inline void HeapReference<T>::assign(ObjPtr<T> ptr) {
  assign(ptr.ptr());
}
