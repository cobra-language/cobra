/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef InterpreterInl_h
#define InterpreterInl_h

#include "cobra/VM/Interpreter.h"

namespace cobra {
namespace vm {

inline double doDiv(double x, double y) {
  // UBSan will complain about float divide by zero as our implementation
  // of OpCode::Div depends on IEEE 754 float divide by zero. All modern
  // compilers implement this and there is no trivial work-around without
  // sacrificing performance and readability.
  return x / y;
}

inline double doMod(double x, double y) {
  // We use fmod here for simplicity. Theoretically fmod behaves slightly
  // differently than the ECMAScript Spec. fmod applies round-towards-zero for
  // the remainder when it's not representable by a double; while the spec
  // requires round-to-nearest. As an example, 5 % 0.7 will give
  // 0.10000000000000031 using fmod, but using the rounding style described by
  // the spec, the output should really be 0.10000000000000053. Such difference
  // can be ignored in practice.
  return std::fmod(x, y);
}

/// \return the product of x multiplied by y.
inline double doMul(double x, double y) {
  return x * y;
}

/// \return the difference of y subtracted from x.
inline double doSub(double x, double y) {
  return x - y;
}

inline int32_t doBitAnd(int32_t x, int32_t y) {
  return x & y;
}

inline int32_t doBitOr(int32_t x, int32_t y) {
  return x | y;
}

inline int32_t doBitXor(int32_t x, int32_t y) {
  return x ^ y;
}

inline int32_t doLShift(uint32_t x, uint32_t y) {
  return x << y;
}

inline int32_t doRShift(int32_t x, uint32_t y) {
  return x >> y;
}

inline uint32_t doURshift(uint32_t x, uint32_t y) {
  return x >> y;
}

inline double doInc(double d) {
  return d + 1;
}

inline double doDec(double d) {
  return d - 1;
}


}
} 
#endif /* InterpreterInl_h */
