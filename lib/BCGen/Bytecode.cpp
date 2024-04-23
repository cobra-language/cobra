/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/Bytecode.h"

using namespace cobra;

void BytecodeModule::setFunction(
    uint32_t index,
    std::unique_ptr<BytecodeFunction> F) {
  assert(index < getNumFunctions() && "Function ID out of bound");
  functions_[index] = std::move(F);
}

BytecodeFunction &BytecodeModule::getFunction(unsigned index) {
  assert(index < getNumFunctions() && "Function ID out of bound");
  assert(functions_[index] && "Invalid function");
  return *functions_[index];
}
