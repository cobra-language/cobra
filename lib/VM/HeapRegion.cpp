/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/HeapRegion.h"
#include "cobra/Support/Common.h"

using namespace cobra;
using namespace vm;

/// Ref arkcompiler BumpPointerAllocator::Allocate
/// and art RegionSpace::Region::Alloc
/// and hermes AlignedHeapSegment::alloc
void *HeapRegion::alloc(size_t size) {
  assert(isSizeHeapAligned(size) && "size must be heap aligned");
  
  uint8_t *oldTop;
  uint8_t *newTop;
  do {
    oldTop = top_.load(std::memory_order_relaxed);
    newTop = oldTop + size;
    if (COBRA_UNLIKELY(newTop > end_)) {
      return nullptr;
    }
  } while(!top_.compare_exchange_weak(oldTop, newTop, std::memory_order_relaxed));
  
  objectsAllocated_.fetch_add(1, std::memory_order_relaxed);
  
  return reinterpret_cast<Object *>(oldTop);
}
