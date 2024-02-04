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

#if __cplusplus > 201402L && __has_cpp_attribute(nodiscard)
#define COBRA_NODISCARD [[nodiscard]]
#elif !__cplusplus
// Workaround for llvm.org/PR23435, since clang 3.6 and below emit a spurious
// error when __has_cpp_attribute is given a scoped attribute in C mode.
#define COBRA_NODISCARD
#elif __has_cpp_attribute(clang::warn_unused_result)
#define COBRA_NODISCARD [[clang::warn_unused_result]]
#else
#define COBRA_NODISCARD
#endif

#ifdef __GNUC__
#define LLVM_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define LLVM_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define LLVM_ATTRIBUTE_NORETURN
#endif

#define FATAL_ERROR(fmt) \
  do { fprintf(stderr, "FATAL ERROR in %s:%d:%s(): " fmt "\n", __FILE__, \
      __LINE__, __func__); exit(-2); } while (0)

#define FATAL_ERRORF(fmt, ...) \
  do { fprintf(stderr, "FATAL ERROR in %s:%d:%s(): " fmt "\n", __FILE__, \
      __LINE__, __func__, __VA_ARGS__); exit(-2); } while (0)

#define TODO do { \
    fprintf(stderr, "Exit due to TODO in %s:%d:%s()\n", __FILE__, __LINE__, __func__); \
    abort(); \
    } while (0)
