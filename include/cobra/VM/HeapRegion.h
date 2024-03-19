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

namespace cobra {
namespace vm {

class HeapRegion {
public:
  HeapRegion() = default;
  
  inline bool alloc(uint32_t size);
  
  void beginMarking();
  
  void endMarking();
    
};

}
}

#endif /* HeapRegion_h */
