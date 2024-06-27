/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/Support/OSCompat.h"
#include "cobra/Support/MathExtras.h"

#include <cassert>
#include <fstream>
#include <vector>

#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <unistd.h>

#ifdef __MACH__
#include <mach/mach.h>

#ifdef __APPLE__
#include <pthread.h>
#endif // __APPLE__

#endif // __MACH__

namespace cobra {

static inline size_t page_size_real() {
  return getpagesize();
}

size_t page_size() {
  return page_size_real();
}

static void *vm_mmap(void *addr, size_t sz, int prot, int flags, bool checkDebugLimit) {
  assert(sz % page_size_real() == 0);
  
  void *result = mmap(addr, sz, prot, flags, -1, 0);
  if (result == MAP_FAILED) {
    return nullptr;
  }
  return result;
}

static void vm_munmap(void *addr, size_t sz) {
  auto ret = munmap(addr, sz);
  assert(!ret && "Failed to free memory region.");
  (void)ret;
}

static char *alignAlloc(void *p, size_t alignment) {
  return reinterpret_cast<char *>(
      alignTo(reinterpret_cast<uintptr_t>(p), alignment));
}

static constexpr int kVMAllocateFlags = MAP_PRIVATE | MAP_ANONYMOUS;
static constexpr int kVMAllocateProt = PROT_READ | PROT_WRITE;

static void * vm_mmap_aligned(void *addr, size_t sz, size_t alignment, int prot, int flags) {
  assert(sz > 0 && sz % page_size() == 0);
  assert(alignment > 0 && alignment % page_size() == 0);

  // Allocate a larger section to ensure that it contains  a subsection that
  // satisfies the request. Use *real* page size here since that's what vm_mmap
  // guarantees.
  const size_t excessSize = sz + alignment - page_size_real();
  auto result = vm_mmap(addr, excessSize, prot, flags, true);
  if (!result)
    return result;

  void *raw = result;
  char *aligned = alignAlloc(raw, alignment);
  size_t excessAtFront = aligned - static_cast<char *>(raw);
  size_t excessAtBack = excessSize - excessAtFront - sz;

  if (excessAtFront)
    vm_munmap(raw, excessAtFront);
  if (excessAtBack)
    vm_munmap(aligned + sz, excessAtBack);

  return aligned;
}

void *vm_allocate(size_t sz, void *hint) {
  assert(sz % page_size() == 0);
  
  return vm_mmap(hint, sz, kVMAllocateProt, kVMAllocateFlags, true);
}

void * vm_allocate_aligned(size_t sz, size_t alignment, void *hint) {
  assert(sz > 0 && sz % page_size() == 0);
  assert(alignment > 0 && alignment % page_size() == 0);

  // Opportunistically allocate without alignment constraint,
  // and see if the memory happens to be aligned.
  // While this may be unlikely on the first allocation request,
  // subsequent allocation requests have a good chance.
  auto result = vm_mmap(hint, sz, kVMAllocateProt, kVMAllocateFlags, true);
  if (!result) {
    return result;
  }
  void *mem = result;
  if (mem == alignAlloc(mem, alignment)) {
    return mem;
  }

  // Free the opportunistic allocation.
  vm_munmap(mem, sz);

  return vm_mmap_aligned(hint, sz, alignment, kVMAllocateProt, kVMAllocateFlags);
}

void vm_free(void *p, size_t sz) {
  vm_munmap(p, sz);
}

void vm_free_aligned(void *p, size_t sz) {
  vm_free(p, sz);
}

static constexpr int kVMReserveProt = PROT_NONE;
static constexpr int kVMReserveFlags =
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE;

void *vm_reserve_aligned(size_t sz, size_t alignment, void *hint) {
  return vm_mmap_aligned(hint, sz, alignment, kVMReserveProt, kVMReserveFlags);
}

void vm_release_aligned(void *p, size_t sz) {
  vm_munmap(p, sz);
}

void *vm_commit(void *p, size_t sz) {
  return vm_mmap(p, sz, kVMAllocateProt, kVMAllocateFlags | MAP_FIXED, false);
}

void vm_uncommit(void *p, size_t sz) {
  auto res = vm_mmap(p, sz, kVMReserveProt, kVMReserveFlags | MAP_FIXED, false);
  (void)res;
  assert(res && "uncommit failed");
}

void vm_hugepage(void *p, size_t sz) {
  assert(
      reinterpret_cast<uintptr_t>(p) % page_size() == 0 &&
      "Precondition: pointer is page-aligned.");

#if defined(__linux__) || defined(__ANDROID__)
  // Since the alloc is aligned, it may benefit from huge pages.
  madvise(p, sz, MADV_HUGEPAGE);
#endif
}

void vm_unused(void *p, size_t sz) {
#ifndef NDEBUG
  const size_t PS = page_size();
  assert(
      reinterpret_cast<intptr_t>(p) % PS == 0 &&
      "Precondition: pointer is page-aligned.");
#endif

/// Change the flag we pass to \p madvise based on the platform, so that we are
/// always acting to reduce memory pressure, as perceived by that platform.
#if defined(__MACH__)

/// On the mach kernel, \p MADV_FREE causes the OS to deduct this memory from
/// the process's physical footprint.
#define MADV_UNUSED MADV_FREE

#elif defined(__linux__)

/// On linux, telling the OS that we \p MADV_DONTNEED some pages will cause it
/// to immediately deduct their size from the process's resident set.
#define MADV_UNUSED MADV_DONTNEED

#else
#error "Don't know how to return memory to the OS on this platform."
#endif // __MACH__, __linux__

  madvise(p, sz, MADV_UNUSED);

#undef MADV_UNUSED
}

void vm_prefetch(void *p, size_t sz) {
  assert(
      reinterpret_cast<intptr_t>(p) % page_size() == 0 &&
      "Precondition: pointer is page-aligned.");
  madvise(p, sz, MADV_WILLNEED);
}

void vm_name(void *p, size_t sz, const char *name) {
#ifdef __ANDROID__
  prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, p, sz, name);
#else
  (void)p;
  (void)sz;
  (void)name;
#endif // __ANDROID__
}

bool vm_protect(void *p, size_t sz, ProtectMode mode) {
  auto prot = PROT_NONE;
  if (mode == ProtectMode::ReadWrite) {
    prot = PROT_WRITE | PROT_READ;
  }
  int err = mprotect(p, sz, prot);
  return err != -1;
}

bool vm_madvise(void *p, size_t sz, MAdvice advice) {
  int param = MADV_NORMAL;
  switch (advice) {
    case MAdvice::Random:
      param = MADV_RANDOM;
      break;
    case MAdvice::Sequential:
      param = MADV_SEQUENTIAL;
      break;
  }
  return madvise(p, sz, param) == 0;
}

bool set_env(const char *name, const char *value) {
  // Enforce the contract of this function that value must not be empty
  assert(*value != '\0' && "value cannot be empty string");
  return setenv(name, value, 1) == 0;
}

bool unset_env(const char *name) {
  return unsetenv(name) == 0;
}

}

