/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RegAlloc_h
#define RegAlloc_h

#include "cobra/IR/IR.h"
#include "cobra/Support/BitVector.h"

#include <map>

namespace cobra {


static const unsigned DEFAULT_REGISTER_COUNT = 30;
using VirtualRegister = unsigned;

class VirtualRegisters {
  BitVector registers{DEFAULT_REGISTER_COUNT};
  
public:
  VirtualRegisters() = default;
  
  VirtualRegister allocateRegister();
};

struct LiveRange {
  size_t start_;
  size_t end_;
  
  explicit LiveRange(size_t start, size_t end) : start_(start), end_(end) {
    assert(end_ >= start_ && "invalid segment range");
  }
  
  bool contains(size_t point) const {
    return point < end_ && point >= start_;
  }
  
  bool contains(LiveRange other) const {
    return (start_ <= other.start_ && other.end_ <= end_);
  }
  
  bool intersects(LiveRange other) const {
    return !(other.start_ >= end_ || start_ >= other.end_);
  }
  
  bool touches(LiveRange other) const {
    return !(other.start_ > end_ || start_ > other.end_);
  }
  
  void merge(LiveRange other) {
    assert(touches(other) && "merging non overlapping LiveRange");
    start_ = std::min(start_, other.start_);
    end_ = std::max(end_, other.end_);
  }
  
  bool operator==(const LiveRange& other) const {
    return std::tie(start_, end_) == std::tie(other.start_, other.end_);
  }
  
  bool operator!=(const LiveRange& other) const { return !(*this == other); }

  bool operator<(const LiveRange& other) const {
    return start_ < other.start_;
  }
  
};

struct LiveInterval {
  std::vector<LiveRange> ranges_;
  
  explicit LiveInterval() = default;
  
  void add(LiveRange other) {
    for (auto &r : ranges_) {
      if (r.touches(other)) {
        r.merge(other);
      }
    }
    ranges_.push_back(other);
  }
  
  size_t start() const {
    assert(ranges_.size() && "No LiveRanges in interval!");
    size_t start = ranges_[0].start_;
    for (auto &r : ranges_) {
      start = std::min(start, r.start_);
    }
    return start;
  }

  size_t end() const {
    assert(ranges_.size() && "No LiveRanges in interval!");
    size_t start = ranges_[0].end_;
    for (auto &r : ranges_) {
      start = std::max(start, r.end_);
    }
    return start;
  }
};

class VirtualRegisterAllocator {
  
  std::map<Value *, VirtualRegister> allocatedReg;
  
  Function *F;
  
  void resolvePhis(std::vector<BasicBlock *> order);
  
  void calculateLocalLiveness();
  
  void calculateGlobalLiveness();
  
  void coalesce(std::vector<BasicBlock *> order);
  
public:
  explicit VirtualRegisterAllocator(Function *func) : F(func) {}

  virtual ~VirtualRegisterAllocator() = default;
  
  void allocate();
  
  VirtualRegister getRegister(Value *I);
  
  bool isAllocated(Value *I);
  
  
};

}

#endif
