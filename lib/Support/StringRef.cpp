//===- StringRef.h - Constant String Reference Wrapper ----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "cobra/Support/StringRef.h"

using namespace cobra;

size_t StringRef::count(StringRef Str) const {
  size_t Count = 0;
  size_t N = Str.size();
  if (N > Length)
    return 0;
  for (size_t i = 0, e = Length - N + 1; i != e; ++i)
    if (substr(i, N).equals(Str))
      ++Count;
  return Count;
}
