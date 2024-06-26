/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef HeapRegionSpace_h
#define HeapRegionSpace_h

#include <stdint.h>
#include <list>
#include "cobra/VM/HeapRegion.h"

namespace cobra {
namespace vm {

class HeapRegionSpace {
  
public:
  static HeapRegionSpace* create(const std::string& name);
  
  /// Ref arkcompiler HeapRegionAllocator::AllocateAlignedRegion
  /// and art RegionSpace::AllocateRegion
  /// and hermes HadesGC::createSegment
  HeapRegion *allocRegion();
  
  HeapRegion *getCurrentRegion() const {
      return regions_.back();
  }

  HeapRegion *getFirstRegion() const {
     return regions_.front();
  }
  
  uint32_t getRegionCount() {
    return regions_.size();
  }
  
  std::list<HeapRegion *> &getRegions() {
    return regions_;
  }

  const std::list<HeapRegion *> &getRegions() const {
    return regions_;
  }
  
private:
  std::list<HeapRegion *> regions_;
};

}

}

#endif /* HeapRegion_h */
