/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RuntimeGlobals_h
#define RuntimeGlobals_h

#include <string>
#include "cobra/Support/MathExtras.h"
#pragma GCC diagnostic push

#ifdef HERMES_COMPILER_SUPPORTS_WSHORTEN_64_TO_32
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#endif

namespace cobra {

/// A type big enough to accomodate the entire allocated address space.
/// Individual allocations are always 'uint32_t', but on a 64-bit machine we
/// might want to accommodate a larger total heap (or not, in which case we keep
/// it 32-bit).
using gcheapsize_t = uint32_t;

static const uint32_t LogHeapAlign = 3;
static const uint32_t HeapAlign = 1 << LogHeapAlign;

/// Align requested size according to the alignment requirement of the GC.
constexpr inline uint32_t heapAlignSize(gcheapsize_t size) {
  return alignTo<HeapAlign>(size);
}

/// Return true if the requested size is properly aligned according to the
/// alignment requirement of the GC.
constexpr inline bool isSizeHeapAligned(gcheapsize_t size) {
  return (size & (HeapAlign - 1)) == 0;
}

using ObjectPointerType = uint32_t;
static constexpr size_t ObjectPointerSize = sizeof(ObjectPointerType);

enum class PointerSize : size_t {
  k32 = 4,
  k64 = 8
};

static constexpr PointerSize kRuntimePointerSize = sizeof(void*) == 8U
? PointerSize::k64
: PointerSize::k32;

/// A constexpr value that is highly likely (but not absolutely
/// guaranteed) to be at least as large, and a multiple of,
/// oscompat::page_size().  Thus, this value can be used to compute
/// page-aligned sizes statically.  However, operations that require
/// actual page_size()-alignment for correctness (e.g., ensuring that
/// mprotect only protects what is intended) should verify the
/// correctness of this value, using the function below.
constexpr size_t kExpectedPageSize =
#if defined(__APPLE__)
    16 * 1024
#else
    4 * 1024
#endif
    ;

}

#endif /* RuntimeGlobals_h */
