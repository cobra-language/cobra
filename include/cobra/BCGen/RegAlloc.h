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
//using VirtualRegister = unsigned;

struct VirtualRegister {
  unsigned value{0};
  
  unsigned getIndex() const {
    return value;
  }
  
  bool operator==(VirtualRegister RHS) const {
    return value == RHS.value;
  }
  bool operator!=(VirtualRegister RHS) const {
    return !(*this == RHS);
  }
};

class VirtualRegisterManager {
  BitVector registers;
  
public:
  VirtualRegisterManager() = default;
  /// \returns true if the register \r is used;
  bool isUsed(VirtualRegister r);

  /// \returns true if the register \r is free;
  bool isFree(VirtualRegister r);
  
  VirtualRegister allocateRegister();
  
  void killRegister(VirtualRegister reg);
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
  
  explicit LiveInterval(size_t start, size_t end) {
    add(LiveRange(start, end));
  }
  
  /// \return true if this interval intersects \p other.
  bool intersects(LiveRange other) const {
    for (auto &r : ranges_) {
      if (r.intersects(other))
        return true;
    }
    return false;
  }

  /// \return true if this interval intersects \p other.
  bool intersects(const LiveInterval &other) const {
    for (auto &r : ranges_) {
      if (other.intersects(r))
        return true;
    }
    return false;
  }
  
  void add(const LiveInterval &other) {
    for (auto &R : other.ranges_) {
      add(R);
    }
  }
  
  void add(LiveRange other) {
    for (auto &r : ranges_) {
      if (r.touches(other)) {
        r.merge(other);
      }
    }
    ranges_.push_back(other);
  }
  
  /// \returns a new compressed interval.
  LiveInterval compress() const {
    LiveInterval t;
    for (auto &r : ranges_) {
      t.add(r);
    }
    return t;
  }

  /// \returns the size represented by the interval.
  size_t size() const {
    if (ranges_.size())
      return end() - start();

    return 0;
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
  /// Represents the liveness info for one block.
  struct BlockLifetimeInfo {
    BlockLifetimeInfo() = default;
    void init(unsigned size) {
      gen_.resize(size);
      kill_.resize(size);
      liveIn_.resize(size);
      liveOut_.resize(size);
      maskIn_.resize(size);
    }
    /// Which live values are used in this block.
    BitVector gen_;
    /// Which live values are defined in this block.
    BitVector kill_;
    /// Which values are marked as live-in, coming into this basic block.
    BitVector liveIn_;
    /// Which values are marked as live-in, coming out of this basic block.
    BitVector liveOut_;
    /// Which values are *masked* as live-in, coming into this basic block. The
    /// mask-in bit vector is used for blocking the flow in specific blocks.
    /// We use this to block the flow of phi values and enforce flow-sensitive
    /// liveness.
    BitVector maskIn_;
  };
  
  /// Maps active slots (per bit) for each basic block.
  std::map<BasicBlock *, BlockLifetimeInfo> blockLiveness_;
  
  /// Maps index numbers to instructions.
  std::map<Instruction *, unsigned> instructionNumbers_;
  /// Maps instructions to a index numbers.
  std::vector<Instruction *> instructionsByNumbers_;
  /// Holds the live interval of each instruction.
  std::vector<LiveInterval> instructionInterval_;
  
  std::map<Value *, VirtualRegister> allocatedReg;
  
  Function *F;
  
  VirtualRegisterManager registerManager{};
  
  /// Returns the last index allocated.
  unsigned getMaxInstrIndex() {
    return instructionsByNumbers_.size();
  }
  
  /// \returns the index of instruction \p I.
  unsigned getInstructionNumber(Instruction *I);

  /// \returns true if the instruction \p already has a number.
  bool hasInstructionNumber(Instruction *I);
  
  void resolvePhis(std::vector<BasicBlock *> &order);
  
  void calculateLocalLiveness(BlockLifetimeInfo &livenessInfo, BasicBlock *BB);
  
  void calculateGlobalLiveness(std::vector<BasicBlock *> &order);
  
  void calculateLiveIntervals(std::vector<BasicBlock *> &order);
  
  void coalesce(std::map<Instruction *, Instruction *> &map, std::vector<BasicBlock *> &order);
  
  void handleInstruction(Instruction *I);
  
public:
  explicit VirtualRegisterAllocator(Function *func) : F(func) {}

  virtual ~VirtualRegisterAllocator() = default;
  
  void allocate();
  
  VirtualRegister getRegister(Value *I);
  
  void updateRegister(Value *I, VirtualRegister R);
  
  bool isAllocated(Value *I);
  
  
};

}

#endif
