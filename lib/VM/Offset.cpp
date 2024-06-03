/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <ostream>
#include "cobra/VM/Offset.h"

using namespace cobra;
using namespace vm;

std::ostream& operator<<(std::ostream& os, const Offset& offs) {
  return os << offs.int32Value();
}
