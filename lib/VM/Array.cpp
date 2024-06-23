/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/Array.h"
#include "cobra/VM/ObjectAccessor.h"

using namespace cobra;
using namespace vm;

template <class T, bool IsVolatile /* = false */>
T Array::getPrimitive(size_t offset) const {
  return ObjectAccessor::getPrimitive<T, IsVolatile>(this, getDataOffset() + offset);
}

template <class T, bool IsVolatile /* = false */>
void Array::setPrimitive(size_t offset, T value) {
  ObjectAccessor::setPrimitive<T, IsVolatile>(this, getDataOffset() + offset, value);
}

template <bool IsVolatile /* = false */>
Object *Array::getObject(int offset) const {
  
}

template <bool IsVolatile /* = false */>
void Array::setObject(size_t offset, Object *value) {
  
}

template <class T, bool needWriteBarrier /* = true */>
void Array::set(uint32_t idx, T value) {
  constexpr bool isREF = std::is_pointer_v<T> && std::is_base_of_v<Object, std::remove_pointer_t<T>>;
  static_assert(std::is_arithmetic_v<T> || isREF, "T should be arithmetic type or pointer to managed object type");
  
  size_t elementSize = isREF ? ObjectPointerSize : sizeof(T);
  size_t offset = elementSize * idx;
  
  if constexpr (isREF) {
    ObjectAccessor::setObject<false, needWriteBarrier>(this, getDataOffset() + offset, value);
  } else {
    ObjectAccessor::setPrimitive(this, getDataOffset() + offset, value);
  }
}

template <class T, bool needReadBarrier /* = true */>
T Array::get(uint32_t idx) const {
  
}
