/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef OSCompat_h
#define OSCompat_h

#ifdef _WINDOWS
#include <io.h>
// Expose alloca to whoever includes OSCompat.h
// On Windows, malloc.h defines alloca (alloca is used in Runtime.cpp
// in order to achieve non-deterministic stack depth).
#include <malloc.h>
#else
#include <unistd.h>
#endif

#include <chrono>
#include <cmath>
#include <cstddef>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace cobra {
namespace oscompat {

/// Returns the current page size.
size_t page_size();

// Allocates a virtual memory region of the given size (required to be
// a multiple of page_size()), and returns a pointer to the start.
// Optionally specify a page-aligned hint for where to place the mapping.
// Returns nullptr if the allocation is unsuccessful.  The pages
// will be zero-filled on demand.
void *vm_allocate(size_t sz, void *hint = nullptr);

// Allocates a virtual memory region of the given size and alignment (both
// must be multiples of page_size()), and returns a pointer to the start.
// Optionally specify a page-aligned hint for where to place the mapping.
// Returns nullptr if the allocation is unsuccessful.  The pages
// will be zero-filled on demand.
void *vm_allocate_aligned(size_t sz, size_t alignment, void *hint = nullptr);

/// Free a virtual memory region allocated by \p vm_allocate.
/// \p p must point to the base address that was returned by \p vm_allocate.
/// Memory region returned by \p vm_allocate_aligned must be freed by
/// invoking \p vm_free_aligned, instead of this function.
/// \p size must match the value passed to the respective allocate functions.
/// In other words, partial free is not allowed.
void vm_free(void *p, size_t sz);

/// Similar to \p vm_free, but for memory regions returned by
/// \p vm_allocate_aligned.
void vm_free_aligned(void *p, size_t sz);

/// Similar to vm_allocate_aligned, but regions of memory must be explicitly
/// committed with \p vm_commit before they are used. This can be used to
/// reserve large contiguous address spaces without failing due to overcommit.
void *vm_reserve_aligned(size_t sz, size_t alignment, void *hint = nullptr);

/// Similar to \p vm_free, but for memory regions returned by
/// \p vm_reserve_aligned.
void vm_release_aligned(void *p, size_t sz);

/// Commit a region of memory so that it can be used. \p sz must be page aligned
/// and \p p must be a page aligned pointer in a region returned by
/// \p vm_reserve_aligned. The pages will be zero-filled on demand.
void *vm_commit(void *p, size_t sz);

/// Uncommit a region of memory once it is no longer in use. \p sz must be page
/// aligned and \p p must be a page aligned pointer in a region returned by
/// \p vm_reserve_aligned.
void vm_uncommit(void *p, size_t sz);

/// Mark the \p sz byte region of memory starting at \p p as being a good
/// candidate for huge pages.
/// \pre sz must be a multiple of oscompat::page_size().
void vm_hugepage(void *p, size_t sz);

/// Mark the \p sz byte region of memory starting at \p p as not currently in
/// use, so that the OS may free it. \p p must be page-aligned.
void vm_unused(void *p, size_t sz);

/// Mark the \p sz byte region of memory starting at \p p as soon being needed,
/// so that the OS may prefetch it. \p p must be page-aligned.
void vm_prefetch(void *p, size_t sz);

/// Assign a \p name to the \p sz byte region of virtual memory starting at
/// pointer \p p.  The name is assigned only on supported platforms (currently
/// only Android).  This name appears when the OS is queried about the mapping
/// for a process (e.g. by /proc/<pid>/maps).
void vm_name(void *p, size_t sz, const char *name);

enum class ProtectMode { ReadWrite, None };

bool vm_protect(void *p, size_t sz, ProtectMode mode);

enum class MAdvice { Random, Sequential };

/// Issue an madvise() call.
/// \return true on success, false on error.
bool vm_madvise(void *p, size_t sz, MAdvice advice);

/// Set the env var \p name to \p value.
/// \p value must not be an empty string.
/// Setting an env var to empty is not supported because doing it
/// cross-platform is hard.
/// \return true if successful, false on error.
bool set_env(const char *name, const char *value);

/// Unset the env var \p name.
/// \return true if successful, false on error.
bool unset_env(const char *name);

}
}

#endif /* OSCompat_h */
