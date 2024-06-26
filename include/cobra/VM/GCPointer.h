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
#include <atomic>
#include <type_traits>

#include "cobra/Support/StdLibExtras.h"

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
    return reinterpret_cast<T *>(reference_);
  }
  
  /// Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<T> downCast(ObjPtr<SourceType> ptr);

  /// Static function to be friendly with null pointers.
  template <typename SourceType>
  static ObjPtr<T> downCast(SourceType *ptr);
};

template<class T>
class PtrCompression {
public:
  /// Compress reference to its bit representation.
  static uint32_t compress(T *ptr) {
    uint32_t bits = reinterpret_cast32<uint32_t>(ptr);
    return bits;
  }

  /// Uncompress an encoded reference from its bit representation.
  static T* decompress(uint32_t ref) {
    uint32_t bits = ref;
    return reinterpret_cast32<T *>(bits);
  }

  /// Convert an ObjPtr to a compressed reference.
  static uint32_t compress(ObjPtr<T> ptr);
};

/// Value type representing a reference to a Object of type T.
template<class T>
class ObjectReference {
private:
  using Compression = PtrCompression<T>;
  
protected:
  explicit ObjectReference(T *ptr) : reference_(Compression::compress(ptr)) {}
  
  ObjectReference() : reference_(0u) {}

  uint32_t reference_;

public:
  /*
   * Returns a pointer to the mirror of the managed object this reference is for.
   *
   * This does NOT return the current object (which isn't derived from, and
   * therefor cannot be a Object) as a mirror pointer. Instead, this
   * returns a pointer to the mirror of the managed object this refers to.
   */
  T *getPtr() const {
    return Compression::decompress(reference_);
  }

  void assign(T* other) {
    reference_ = Compression::compress(other);
  }

  void assign(ObjPtr<T> ptr);

  void clear() {
    reference_ = 0;
  }

  bool isNull() const {
    return reference_ == 0;
  }

  static ObjectReference<T> fromPtr(T *ptr) {
    return ObjectReference<T>(ptr);
  }
};

/// References between objects within the managed heap.
/// Similar API to ObjectReference, but not a value type. Supports atomic access.
template<class T>
class HeapReference {
private:
  using Compression = PtrCompression<T>;
  
  explicit HeapReference(T *ptr) : reference_(Compression::compress(ptr)) {}

  std::atomic<uint32_t> reference_;

public:
  HeapReference() : HeapReference(nullptr) {}

  T *getPtr() const  {
    return Compression::decompress(reference_.load(std::memory_order_relaxed));
  }

  void assign(T *other) {
    reference_.store(Compression::compress(other), std::memory_order_relaxed);
  }

  void assign(ObjPtr<T> ptr);

  void clear() {
    reference_.store(0, std::memory_order_relaxed);
  }

  bool isNull() const {
    return reference_.load(std::memory_order_relaxed) == 0;
  }

  static HeapReference<T> fromPtr(T *ptr) {
    return HeapReference<T>(ptr);
  }
};

class CompressedPointer {
public:
  using StorageType = uint32_t;
  
  explicit CompressedPointer() = default;
  constexpr explicit CompressedPointer(std::nullptr_t) : ptr_(0u) {}
  
  explicit CompressedPointer(StorageType s) : ptr_(s) {}
  
  CompressedPointer(const CompressedPointer &) = default;
  
  template <typename T>
  static CompressedPointer encode(T *ptr) {
    CompressedPointer(compress(ptr));
  }
  
  template <typename T>
  static StorageType compress(T *ptr) {
    return reinterpret_cast32<uint32_t>(ptr);
  }
  
  template <typename T>
  static T *decompress(StorageType ptr) {
    return reinterpret_cast32<T *>(ptr);
  }
  
  template <typename T>
  T *get() const {
    return reinterpret_cast<T *>(ptr_);
  }
  
  template <typename T>
  void set(T *ptr) {
    ptr_ = CompressedPointer::encode(ptr).ptr_;
  }
  
  StorageType getRaw() const {
    return ptr_;
  }
  
  void clear() { ptr_ = 0u; }
  bool isCleared() const { return !ptr_; }
  
  bool operator==(const CompressedPointer &other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const CompressedPointer &other) const {
    return !(*this == other);
  }
  
protected:
 void setNoBarrier(CompressedPointer cp) {
   ptr_ = cp.ptr_;
 }
  
private:
  StorageType ptr_;
  
};

template <typename T>
class GCPointer : public CompressedPointer{
public:
  GCPointer() : CompressedPointer(nullptr) {}
  GCPointer(std::nullptr_t) : CompressedPointer(nullptr) {}
  
  GCPointer(T *ptr) : CompressedPointer(CompressedPointer::compress<T>(ptr)) {};
  
  /// We are not allowed to copy-construct or assign GCPointers.
  GCPointer(const GCPointer<T> &) = delete;
  GCPointer &operator=(const GCPointer<T> &) = delete;
  
  void set(T *ptr) {
    setNoBarrier(CompressedPointer::encode<T>(ptr));
  }
  
};


}
}

#endif /* GCPointer_h */
