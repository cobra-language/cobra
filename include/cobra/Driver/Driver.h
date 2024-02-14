/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Driver_h
#define Driver_h

#include <string>

#include "cobra/Parser/Parser.h"
#include "cobra/IRGen/IRGen.h"

namespace cobra {
namespace driver {

bool compile(std::string source);


} // namespace driver
} // namespace cobra

#endif
