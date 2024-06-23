/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Array_h
#define Array_h

#include "cobra/VM/Object.h"
#include "cobra/Support/Common.h"

namespace cobra {
namespace vm {

class Array : Object {
public:
  static Array *create(Class arrayClass, uint32_t length);
  
  static size_t computeSize(size_t elementSize, uint32_t length) {
      assert(elementSize != 0);
      size_t size = sizeof(Array) + elementSize * length;
      size_t size_limit = (std::numeric_limits<size_t>::max() - sizeof(Array)) / elementSize;
      if (COBRA_UNLIKELY(size_limit < static_cast<size_t>(length))) {
          return 0;
      }
      return size;
  }
  
  uint32_t getLength() const {
    // Atomic with relaxed order reason: data race with length_ with no synchronization or ordering constraints
    // imposed on other reads or writes
    return length_.load(std::memory_order_relaxed);
  }
  
  uint32_t *getData() {
    return data_;
  }

  const uint32_t *getData() const {
    return data_;
  }
  
  template <class T, bool IsVolatile = false>
  T getPrimitive(size_t offset) const;

  template <class T, bool IsVolatile = false>
  void setPrimitive(size_t offset, T value);
  
  template <bool IsVolatile = false, bool needReadBarrier = true>
  Object *getObject(int offset) const;

  template <bool IsVolatile = false, bool needWriteBarrier = true>
  void setObject(size_t offset, Object *value);
  
  template <class T>
  static constexpr size_t getElementSize();
  
  size_t objectSize(uint32_t componentSize) const {
    return computeSize(componentSize, length_);
  }
  
  static constexpr uint32_t getLengthOffset() {
    return MEMBER_OFFSET(Array, length_);
  }
  
  static constexpr uint32_t getDataOffset() {
    return MEMBER_OFFSET(Array, data_);
  }
  
  template <class T, bool needWriteBarrier = true>
  void set(uint32_t idx, T value);
  
  template <class T, bool needReadBarrier = true>
  T get(uint32_t idx) const;
  
  
private:
  void setLength(uint32_t length) {
    // Atomic with relaxed order reason: data race with length_ with no synchronization or ordering constraints
    // imposed on other reads or writes
    length_.store(length, std::memory_order_relaxed);
  }
  
  std::atomic<uint32_t> length_;
  uint32_t data_[0];
};

}
}

#endif /* Array_h */
