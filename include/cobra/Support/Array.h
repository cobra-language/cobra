/*
 *  * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Array_h
#define Array_h

#include <stddef.h>
#include <string.h>

#include "cobra/Support/StdLibExtras.h"
#include "cobra/Support/StrideIterator.h"

namespace cobra {

template<typename T>
class Array {
private:
  uint32_t size_;
  uint8_t data_[0];
  
  T &atUnchecked(size_t index, size_t elementSize, size_t alignment) {
    return *reinterpret_cast<T *>(
      reinterpret_cast<uintptr_t>(this) + offsetOfElement(index, elementSize, alignment));
  }

  const T &atUnchecked(size_t index, size_t elementSize, size_t alignment) const {
    return *reinterpret_cast<T *>(
      reinterpret_cast<uintptr_t>(this) + offsetOfElement(index, elementSize, alignment));
  }
  
public:
  explicit Array(size_t length) : size_(static_cast<uint32_t>(length)) {}
  
  size_t size() const {
    return size_;
  }

  /// Update the length but does not reallocate storage.
  void SetSize(size_t length) {
    size_ = static_cast<uint32_t>(length);
  }
  
  T &at(size_t index, size_t elementSize = sizeof(T), size_t alignment = alignof(T)) {
    return atUnchecked(index, elementSize, alignment);
  }
  
  const T &at(size_t index, size_t elementSize = sizeof(T), size_t alignment = alignof(T)) const {
    return atUnchecked(index, elementSize, alignment);
  }
  
  StrideIterator<T> begin(size_t elementSize = sizeof(T), size_t alignment = alignof(T)) {
    return StrideIterator<T>(&atUnchecked(0, elementSize, alignment), elementSize);
  }

  StrideIterator<const T> begin(
      size_t elementSize = sizeof(T),
      size_t alignment = alignof(T)) const {
    return StrideIterator<const T>(&atUnchecked(0, elementSize, alignment), elementSize);
  }

  StrideIterator<T> end(size_t elementSize = sizeof(T), size_t alignment = alignof(T)) {
    return StrideIterator<T>(&atUnchecked(size_, elementSize, alignment), elementSize);
  }

  StrideIterator<const T> end(
      size_t elementSize = sizeof(T),
      size_t alignment = alignof(T)) const {
    return StrideIterator<const T>(&atUnchecked(size_, elementSize, alignment), elementSize);
  }
  
  static size_t offsetOfElement(
      size_t index,
      size_t elementSize = sizeof(T),
      size_t alignment = alignof(T)) {
    return roundUp(offsetof(Array<T>, data_), alignment) + index * elementSize;
  }
  
  /// Clear the potentially uninitialized padding between the size_ and actual data.
  void clearPadding(size_t elementSize = sizeof(T), size_t alignment = alignof(T)) {
    size_t gapOffset = offsetof(Array<T>, data_);
    size_t gapSize = offsetOfElement(0, elementSize, alignment) - gapOffset;
    memset(reinterpret_cast<uint8_t*>(this) + gapOffset, 0, gapSize);
  }
};

}


#endif /* Array_h */
