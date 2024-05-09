/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef RegAlloc_h
#define RegAlloc_h

#include "cobra/IR/IR.h"

#include <map>

namespace cobra {

using VirtualRegister = unsigned;

class VirtualRegisterAllocator {
  
  std::map<Value *, VirtualRegister> allocatedReg;
  
  Function *F;
  
  void resolvePhis(std::vector<BasicBlock *> order);
  
  void calculateLocalLiveness();
  
  void calculateGlobalLiveness();
  
  void coalesce(std::vector<BasicBlock *> order);
  
public:
  explicit VirtualRegisterAllocator(Function *func) : F(func) {}

  virtual ~VirtualRegisterAllocator() = default;
  
  void allocate();
  
  VirtualRegister getRegister(Value *I);
  
  bool isAllocated(Value *I);
  
  
};

}

#endif
