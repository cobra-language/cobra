/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef MarkBitSet_h
#define MarkBitSet_h

#include "cobra/VM/BitSet.h"
#include "cobra/VM/RuntimeGlobals.h"

namespace cobra {
namespace vm {

class MarkBitSet {
public:
  MarkBitSet() = default;

  /// MarkBitSet is not copyable or movable: It must be constructed
  /// in-place.
  MarkBitSet(const MarkBitSet &) = delete;
  MarkBitSet(MarkBitSet &&) = delete;
  MarkBitSet &operator=(const MarkBitSet &) = delete;
  MarkBitSet &operator=(MarkBitSet &&) = delete;
  
private:
  static constexpr size_t kNumBits = 1;
  BitSet<kNumBits> bitSet;
  
public:
  static constexpr size_t size() {
    return kNumBits;
  }
  
  // 类似JSC candidateAtomNumber 方法
  inline size_t index(const void *ptr) const {
    return (reinterpret_cast<uintptr_t>(ptr) - reinterpret_cast<uintptr_t>(this)) >> LogHeapAlign;
  }
  
  inline bool at(size_t idx) const {
    assert(idx < kNumBits && "precondition: ind must be within the index range");
    return true;
    return bitSet.contains(idx);
  }
  
  inline void mark(size_t idx) {
    assert(idx < kNumBits && "precondition: ind must be within the index range");
    bitSet.set(idx, true);
  }
  
  inline void mark(const void *ptr) {
    size_t idx = index(ptr);
    mark(idx);
  }
  
  inline void clear() {
    bitSet.reset();
  }

  inline void markAll() {
    bitSet.set();
  }
  
};



}
}

#endif /* MarkBitSet_h */
