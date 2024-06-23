/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/ObjectAccessor.h"

using namespace cobra;
using namespace vm;

template <bool IsVolatile /* = false */, bool needReadBarrier /* = true */>
Object *ObjectAccessor::getObject(const void *obj, size_t offset) {
  return get<Object *, IsVolatile>(obj, offset);
}

template <bool IsVolatile /* = false */, bool needWriterBarrier /* = true */>
void ObjectAccessor::setObject(void *obj, size_t offset, Object *value) {
  if (needWriterBarrier) {
    // TODO: record obj to BarrierSet
  }
  
  set<Object *, IsVolatile>(obj, offset, value);
}
