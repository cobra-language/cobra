/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GC_h
#define GC_h

#include <string>

#include "cobra/VM/HeapRegion.h"
#include "cobra/VM/CardTable.h"
#include "cobra/VM/GCRoot.h"

namespace cobra {
namespace vm {

// The CellState of a cell is a kind of hint about what the state of the cell is.
enum class CellState : uint8_t {
  // The object is either currently being scanned, or it has finished being scanned, or this
  // is a full collection and it's actually a white object (you'd know because its mark bit
  // would be clear).
  Black = 0,
  
  // The object is in eden. During GC, this means that the object has not been marked yet.
  White = 1,
  
  // This sorta means that the object is grey - i.e. it will be scanned. Or it could be white
  // during a full collection if its mark bit is clear. That would happen if it had been black,
  // got barriered, and we did a full collection.
  Grey = 2
};

class GC {
  
  enum class Phase : uint8_t {
    Idle,
    Mark,
    CompleteMarking,
    Sweep,
  };
  
public:
  GC() = default;
  
  void writeBarrier(const Object *obj, const Object *value);
  
private:
  
  
};

}
}

#endif /* GC_h */
