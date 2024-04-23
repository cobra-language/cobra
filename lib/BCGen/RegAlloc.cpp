/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/RegAlloc.h"
#include "cobra/IR/IR.h"
#include "cobra/IR/IRBuilder.h"
#include "cobra/IR/Instrs.h"

#include <algorithm>
#include <queue>

using namespace cobra;

VirtualRegister VirtualRegisterAllocator::getRegister(Value *I) {
  assert(isAllocated(I) && "Instruction is not allocated!");
  return allocatedReg[I];
}

bool VirtualRegisterAllocator::isAllocated(Value *I) {
  return allocatedReg.count(I);
}
