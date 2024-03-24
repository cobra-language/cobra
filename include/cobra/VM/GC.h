/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GC_h
#define GC_h

#include "cobra/VM/HeapRegion.h"
#include "cobra/VM/CardTable.h"

namespace cobra {
namespace vm {

// ArkCompiler
enum class GCPhase {
    GC_PHASE_IDLE,  // GC waits for trigger event
    GC_PHASE_RUNNING,
    GC_PHASE_COLLECT_ROOTS,
    GC_PHASE_INITIAL_MARK,
    GC_PHASE_MARK,
    GC_PHASE_MARK_YOUNG,
    GC_PHASE_REMARK,
    GC_PHASE_COLLECT_YOUNG_AND_MOVE,
    GC_PHASE_SWEEP,
    GC_PHASE_CLEANUP,
    GC_PHASE_LAST
};

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
    None,
    Mark,
    CompleteMarking,
    Sweep,
  };
  
public:
  GC() = default;
  
  void writeBarrier(const GCCell *value);
  
private:
  std::unique_ptr<CardTable> cardTable;
};

}
}

#endif /* GC_h */
