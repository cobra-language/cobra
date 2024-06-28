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

void HeapRegion::Contents::protectGuardPage(oscompat::ProtectMode mode) {
  char *begin = &paddedGuardPage_[kGuardPagePadding];
  size_t size = sizeof(paddedGuardPage_) - kGuardPagePadding;
  size_t PS = oscompat::page_size();
  // Only protect if the actual system page size matches expectations.
  if (reinterpret_cast<uintptr_t>(begin) % PS == 0 && PS <= size) {
    vm_protect(begin, PS, mode);
  }
}

HeapRegion::HeapRegion(void *allocateBase) : allocateBase_(static_cast<char *>(allocateBase)) {
  assert(
      reinterpret_cast<uintptr_t>(end()) % oscompat::page_size() == 0 &&
      "storage end must be page-aligned");
  new (contents()) Contents();
  contents()->protectGuardPage(oscompat::ProtectMode::None);
}

/// Ref arkcompiler BumpPointerAllocator::Allocate
/// and art RegionSpace::Region::Alloc
/// and hermes AlignedHeapSegment::alloc
void *HeapRegion::alloc(size_t size) {
  assert(isSizeHeapAligned(size) && "size must be heap aligned");
  char *oldTop;
  char *newTop = top_ + size;
  if (COBRA_UNLIKELY(newTop > end())) {
    return nullptr;
  }
  oldTop = top_;
  top_ = newTop;
  
  return reinterpret_cast<void *>(oldTop);
}
