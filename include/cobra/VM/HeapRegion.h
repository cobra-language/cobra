/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HeapRegion_h
#define HeapRegion_h

#include <stdint.h>

#include "cobra/VM/GCCell.h"
#include "cobra/VM/MarkBitSet.h"
#include "cobra/VM/RuntimeGlobals.h"
#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

static constexpr size_t KB = 1024;

class HeapRegion {
public:
  static constexpr size_t KLogSize = 20;
  static constexpr size_t KSize = {1 << KLogSize};
  static constexpr size_t kMask = ~(KSize - 1);
  
  HeapRegion() = default;
  HeapRegion(HeapRegion &&) = default;
  HeapRegion &operator=(HeapRegion &&) = default;

  ~HeapRegion();
  
  class Header {
    friend class HeapRegion;
    
    MarkBitSet markBitSet;
    MarkBitSet newlyAllocated;
    
  };
  
  inline static Header *header(void *ptr);
  
  inline static HeapRegion *getHeapRegion(const void *ptr);
  
  inline static void *start(const void *ptr);
  
  inline static MarkBitSet *getMarkBitSet(const void *ptr);
  
  inline static void setCellMarkBit(const GCCell *cell);
  
  uint8_t *begin() const {
    return begin_;
  }
  
  uint8_t *top() const {
    return top_.load(std::memory_order_relaxed);
  }

  void setTop(uint8_t* newTop) {
    top_.store(newTop, std::memory_order_relaxed);
  }
  
  uint8_t *end() const {
    return end_;
  }
  
  bool contains(Object* obj) const {
    return begin_ <= reinterpret_cast<uint8_t*>(obj) && reinterpret_cast<uint8_t*>(obj) < end_;
  }
  
  inline void *alloc(size_t size);
  
  void beginMarking();
  
  void endMarking();
  
private:
  /// The begin address of the region.
  uint8_t *begin_;
  /// Note that `top_` can be higher than `end_` in the case of a
  /// large region, where an allocated object spans multiple regions
  /// (large region + one or more large tail regions).
  /// The current position of the allocation.
  std::atomic<uint8_t*> top_;
  
  /// The end address of the region.
  uint8_t *end_;
  
  /// The number of objects allocated.
  std::atomic<size_t> objectsAllocated_;
  
};

/* static */
HeapRegion::Header *HeapRegion::header(void *lowLim) {
  return reinterpret_cast<Header *>(lowLim);
}

/*static*/
HeapRegion *HeapRegion::getHeapRegion(const void *ptr) {
  return reinterpret_cast<HeapRegion *>(reinterpret_cast<uintptr_t>(ptr) & kMask);
}

/* static */ void *HeapRegion::start(const void *ptr) {
  return reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(ptr) & kMask);
}

/*static*/
MarkBitSet *HeapRegion::getMarkBitSet(const void *ptr) {
  return &header(HeapRegion::start(ptr))->markBitSet;
}

/*static*/
void HeapRegion::setCellMarkBit(const GCCell *cell) {
  MarkBitSet *markBits = getMarkBitSet(cell);
  
}

}

}

#endif /* HeapRegion_h */
