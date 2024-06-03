/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef LengthPrefixedArray_h
#define LengthPrefixedArray_h

#include <stddef.h>
#include <string.h>

#include "cobra/Support/MathExtras.h"
#include "cobra/Support/StrideIterator.h"

namespace cobra {

template<typename T>
class LengthPrefixedArray {
private:
  uint32_t size_;
  uint8_t data_[0];
  
  T &atUnchecked(size_t index, size_t elementSize, size_t alignment) {
    return *reinterpret_cast<T*>(
      reinterpret_cast<uintptr_t>(this) + offsetOfElement(index, elementSize, alignment));
  }

  const T &atUnchecked(size_t index, size_t elementSize, size_t alignment) const {
    return *reinterpret_cast<T*>(
      reinterpret_cast<uintptr_t>(this) + offsetOfElement(index, elementSize, alignment));
  }
  
public:
  explicit LengthPrefixedArray(size_t length) : size_(static_cast<uint32_t>(length)) {}
  
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
    return roundUp(offsetof(LengthPrefixedArray<T>, data_), alignment) + index * elementSize;
  }
  
  /// Clear the potentially uninitialized padding between the size_ and actual data.
  void clearPadding(size_t elementSize = sizeof(T), size_t alignment = alignof(T)) {
    size_t gapOffset = offsetof(LengthPrefixedArray<T>, data_);
    size_t gapSize = offsetOfElement(0, elementSize, alignment) - gapOffset;
    memset(reinterpret_cast<uint8_t*>(this) + gapOffset, 0, gapSize);
  }
};

}


#endif /* LengthPrefixedArray_h */
