/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/BytecodeGenerator.h"

using namespace cobra;

std::unique_ptr<BytecodeFunction>
BytecodeFunctionGenerator::generateBytecodeFunction() {
  
}


void BytecodeFunctionGenerator::emitMov(param_t dest, param_t src) {
  
}

unsigned BytecodeGenerator::addFunction(Function *F) {
  functions.push_back(F);
  unsigned index = functions.size();
  functionIDMap[F] = index;
  return index;
}

void BytecodeGenerator::setFunctionGenerator(
    Function *F,
    std::unique_ptr<BytecodeFunctionGenerator> BFG) {
  assert(
      functionGenerators_.find(F) == functionGenerators_.end() &&
      "Adding same function twice.");
  assert(
      !BFG->hasEncodingError() && "Error should have been reported already.");
  functionGenerators_[F] = std::move(BFG);
}

std::unique_ptr<BytecodeModule> BytecodeGenerator::generate() {
  std::unique_ptr<BytecodeModule> BM{new BytecodeModule()};
  
  for (unsigned i = 0, e = functions.size(); i < e; ++i) {
    auto *F = functions[i];
    auto &BFG = *functionGenerators_[F];
    
    std::unique_ptr<BytecodeFunction> func = BFG.generateBytecodeFunction();
    
    BM->setFunction(i, std::move(func));
  }
  return BM;
}
