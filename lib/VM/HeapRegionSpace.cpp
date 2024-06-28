/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <random>

#include "cobra/VM/HeapRegionSpace.h"
#include "cobra/Support/OSCompat.h"

using namespace cobra;
using namespace vm;

bool isAligned(void *p) {
  return (reinterpret_cast<uintptr_t>(p) & (HeapRegion::kSize - 1)) == 0;
}

char *alignAlloc(void *p) {
  return reinterpret_cast<char *>(
      alignTo(reinterpret_cast<uintptr_t>(p), HeapRegion::kSize));
}

void *getMmapHint() {
  uintptr_t addr = std::random_device()();
  if constexpr (sizeof(uintptr_t) >= 8) {
    // std::random_device() yields an unsigned int, so combine two.
    addr = (addr << 32) | std::random_device()();
    // Don't use the entire address space, to prevent too much fragmentation.
    addr &= std::numeric_limits<uintptr_t>::max() >> 18;
  }
  return alignAlloc(reinterpret_cast<void *>(addr));
}

static void *alloc(const char *name) {
  auto result = oscompat::vm_allocate_aligned(HeapRegion::kSize, HeapRegion::kSize, getMmapHint());
  if (!result) {
    return result;
  }
  void *mem = result;
  assert(isAligned(mem));
  
  // Name the memory region on platforms that support naming.
  oscompat::vm_name(mem, HeapRegion::kSize, name);
  return mem;
}

HeapRegion *HeapRegionSpace::allocRegion() {
  auto addr = alloc("cobra-heapregion");
  return new HeapRegion(addr);
}
