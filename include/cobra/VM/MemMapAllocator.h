/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef MemMapAllocator_h
#define MemMapAllocator_h

#include <stdio.h>
#include <cstddef>

namespace cobra {
namespace vm {

class MemMapAllocator {
public:
  MemMapAllocator() = default;
  ~MemMapAllocator() = default;
  
  MemMapAllocator(const MemMapAllocator &) = delete;
  MemMapAllocator(MemMapAllocator &&) = delete;
  MemMapAllocator &operator=(const MemMapAllocator &) = delete;
  MemMapAllocator &operator=(MemMapAllocator &&) = delete;
  
  
  
};

}
}

#endif /* MemMapAllocator_h */
