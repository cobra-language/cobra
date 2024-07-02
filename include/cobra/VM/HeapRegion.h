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
#include "cobra/VM/MarkBitSet.h"
#include "cobra/VM/RuntimeGlobals.h"
#include "cobra/VM/Object.h"
#include "cobra/VM/CardTable.h"
#include "cobra/Support/OSCompat.h"

namespace cobra {
namespace vm {

static constexpr size_t KB = 1024;
static constexpr size_t DEFAULT_HEAP_REGION_SIZE = 4096 * KB;

template <size_t N>
struct ConstantLog2
    : std::integral_constant<size_t, ConstantLog2<N / 2>::value + 1> {};
template <> struct ConstantLog2<1> : std::integral_constant<size_t, 0> {};

/// Ref arkcompiler Region
/// and art Region
/// and hermes AlignedHeapSegment
///
/// An \c HeapRegion is a contiguous chunk of memory aligned to its own
/// storage size (which is a fixed power of two number of bytes).  The storage
/// is further split up according to the diagram below:
///
/// +----------------------------------------+
/// | (1) Card Table                         |
/// +----------------------------------------+
/// | (2) Mark Bit Array                     |
/// +----------------------------------------+
/// | (3) Allocation Space                   |
/// |                                        |
/// | ...                                    |
/// |                                        |
/// | (End)                                  |
/// +----------------------------------------+
///
class HeapRegion {
public:
  /// The size and the alignment of the storage, in bytes.
  static constexpr unsigned kLogSize{ConstantLog2<DEFAULT_HEAP_REGION_SIZE>::value};
  static constexpr size_t kSize{1 << kLogSize};
  
  /// Mask for isolating the offset into a storage for a pointer.
  static constexpr size_t kLowMask{kSize - 1};

  /// Mask for isolating the storage being pointed into by a pointer.
  static constexpr size_t kHighMask{~kLowMask};
  
//  static constexpr size_t KLogSize = 20;
//  static constexpr size_t KSize = {1 << KLogSize};
//  static constexpr size_t kMask = ~(KSize - 1);
  
  /// Returns the pointer to the beginning of the storage containing \p ptr
  /// (inclusive). Assuming such a storage exists. Note that
  ///
  ///     HeapRegion::start(storage.hiLim()) != storage.lowLim()
  ///
  /// as \c storage.hiLim() is not contained in the bounds of \c storage -- it
  /// is the first address not in the bounds.
  inline static void *start(const void *ptr);

  /// Returns the pointer to the end of the storage containing \p ptr
  /// (exclusive). Assuming such a storage exists. Note that
  ///
  ///     HeapRegion::end(storage.hiLim()) != storage.hiLim()
  ///
  /// as \c storage.hiLim() is not contained in the bounds of \c storage -- it
  /// is the first address not in the bounds.
  inline static const void *end(const void *ptr);
  
  HeapRegion(void *allocateBase);

  ~HeapRegion();
  
  class Contents {
    friend class HeapRegion;
    
    CardTable cardTable_;
    MarkBitSet markBitSet_;
    
    static constexpr size_t kMetadataSize =
        sizeof(cardTable_) + sizeof(markBitSet_);
    /// Padding to ensure that the guard page is aligned to a page boundary.
    static constexpr size_t kGuardPagePadding =
        alignTo<kExpectedPageSize>(kMetadataSize) - kMetadataSize;

    /// Memory made inaccessible through protectGuardPage, for security and
    /// earlier detection of corruption. Padded to contain at least one full
    /// aligned page.
    char paddedGuardPage_[kExpectedPageSize + kGuardPagePadding];

    static constexpr size_t kMetadataAndGuardSize =
        kMetadataSize + sizeof(paddedGuardPage_);
    
    /// The first byte of the allocation region, which extends past the "end" of
    /// the struct, to the end of the memory region that contains it.
    char start_[1];
    
  public:
   /// Set the protection mode of paddedGuardPage_ (if system page size allows
   /// it).
    void protectGuardPage(oscompat::ProtectMode mode);
  };
  
  /// Attempt an allocation of the given size in the region.  If there is
  /// sufficent space, cast the space as a Object, and returns an uninitialized
  /// pointer to that object.  If there is not sufficient
  /// space, returns nullptr.
  inline void *alloc(size_t size);
  
  /// Given the \p allocateBase_ of some valid memory region, returns
  /// a pointer to the HeapRegion::Contents laid out in that storage,
  /// assuming it exists.
  inline static Contents *contents(void *start);
  
  /// The largest size the allocation region of an aligned heap segment could
  /// be.
  inline static constexpr size_t maxSize();
  
  char *start() const {
    return contents()->start_;
  }
  
  char *top() const {
    return top_;
  }

  void setTop(void *newTop) {
    top_ = reinterpret_cast<char *>(newTop);
  }
  
  char *end() const {
    return start() + maxSize();
  }
  
  size_t size() const {
    return end() - start();
  }
  
  bool contains(Object* obj) const {
    return allocateBase_ <= reinterpret_cast<char *>(obj) && reinterpret_cast<char *>(obj) < end();
  }
  
  inline CardTable &cardTable() const;

  inline MarkBitSet &markBitSet() const;
  
  /// Ref arkcompiler Region::ObjectAddressToRange
  inline static HeapRegion *getHeapRegion(const void *ptr);
    
  inline static MarkBitSet *getMarkBitSet(const void *ptr);
  
  inline static void setCellMarkBit(const Object *cell);
  
  inline static bool getCellMarkBit(const Object *cell);
  
  inline static CardTable *getCardTable(const void *ptr);
  
private:
  
  inline Contents *contents() const;
    
  /// The begin address of the region.
  char *allocateBase_;
  
  /// Note that `top_` can be higher than `end_` in the case of a
  /// large region, where an allocated object spans multiple regions
  /// (large region + one or more large tail regions).
  /// The current position of the allocation.
  char *top_;
};

HeapRegion::Contents *HeapRegion::contents(void *lowLim) {
  return reinterpret_cast<Contents *>(lowLim);
}

HeapRegion *HeapRegion::getHeapRegion(const void *ptr) {
  return reinterpret_cast<HeapRegion *>(reinterpret_cast<uintptr_t>(ptr) & kHighMask);
}

void *HeapRegion::start(const void *ptr) {
  return reinterpret_cast<char *>(reinterpret_cast<uintptr_t>(ptr) & kHighMask);
}

const void *HeapRegion::end(const void *ptr) {
  return reinterpret_cast<char *>(start(ptr)) + kSize;
}

MarkBitSet *HeapRegion::getMarkBitSet(const void *ptr) {
  return &contents(HeapRegion::start(ptr))->markBitSet_;
}

void HeapRegion::setCellMarkBit(const Object *object) {
  MarkBitSet *markBits = getMarkBitSet(object);
  size_t ind = markBits->index(object);
  markBits->mark(ind);
}

bool HeapRegion::getCellMarkBit(const Object *object) {
  MarkBitSet *markBits = getMarkBitSet(object);
  size_t ind = markBits->index(object);
  return markBits->at(ind);
}

CardTable *HeapRegion::getCardTable(const void *ptr) {
  return &contents(HeapRegion::start(ptr))->cardTable_;
}

constexpr size_t HeapRegion::maxSize() {
  return HeapRegion::kSize - offsetof(Contents, start_);
}

HeapRegion::Contents *HeapRegion::contents() const {
  return contents(allocateBase_);
}

CardTable &HeapRegion::cardTable() const {
  return contents()->cardTable_;
}

MarkBitSet &HeapRegion::markBitSet() const {
  return contents()->markBitSet_;
}

}

}

#endif /* HeapRegion_h */
