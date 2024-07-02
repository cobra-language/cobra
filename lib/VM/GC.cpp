/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/GC.h"

using namespace cobra;
using namespace vm;

void GC::writeBarrier(const Object *obj, const Object *value) {
  HeapRegion::getCardTable(obj)->markCard(obj);
}
