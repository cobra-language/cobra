/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RuntimeGlobals_h
#define RuntimeGlobals_h

#include <string>

using ObjectPointerType = uint32_t;
static constexpr size_t ObjectPointerSize = sizeof(ObjectPointerType);

enum class PointerSize : size_t {
  k32 = 4,
  k64 = 8
};

static constexpr PointerSize kRuntimePointerSize = sizeof(void*) == 8U
                                                       ? PointerSize::k64
                                                       : PointerSize::k32;

#endif /* RuntimeGlobals_h */
