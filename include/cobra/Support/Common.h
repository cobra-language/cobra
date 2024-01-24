/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#ifdef _MSC_VER
#define COBRA_NORETURN __declspec(noreturn)
#define COBRA_NOINLINE __declspec(noinline)
#define COBRA_FORCEINLINE __forceinline
#define COBRA_LIKELY(x) x
#define COBRA_UNLIKELY(x) x
#define COBRA_UNREACHABLE() __assume(false)
#define COBRA_DEBUGBREAK() __debugbreak()
#else
#define COBRA_NORETURN __attribute__((__noreturn__))
#define COBRA_NOINLINE __attribute__((noinline))
#define COBRA_FORCEINLINE inline __attribute__((always_inline))
#define COBRA_LIKELY(x) __builtin_expect(x, 1)
#define COBRA_UNLIKELY(x) __builtin_expect(x, 0)
#define COBRA_UNREACHABLE() __builtin_unreachable()
#define COBRA_DEBUGBREAK() __builtin_trap()
#endif
