/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/HeapRegion.h"

using namespace cobra;
using namespace vm;

void *HeapRegion::alloc(uint32_t size) {
  assert(isSizeHeapAligned(size) && "size must be heap aligned");
  
  char *objPtr;
  
  return objPtr;
}
