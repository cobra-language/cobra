/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/BCGen/BCPasses.h"
#include "cobra/IR/IRBuilder.h"

using namespace cobra;

bool LoadConstants::runOnFunction(Function *F) {
  IRBuilder builder(F);
  builder.setInsertionPoint(F->front()->front());
  
  bool changed = false;
  
  std::map<Literal *, Instruction *> constMap{};
  
  auto createLoadLiteral = [&builder](Literal *literal) -> Instruction * {
    return dynamic_cast<Instruction *>(builder.createLoadConstInst(literal));
  };
  
  F->dump();
  
  for (auto BB : F->getBasicBlockList()) {
    for (auto &it : BB->getInstList()) {
      for (unsigned i = 0, n = it->getNumOperands(); i < n; i++) {

        auto *operand = dynamic_cast<Literal *>(it->getOperand(i));
        if (!operand)
          continue;

        auto load = createLoadLiteral(operand);

        it->setOperand(load, i);
        changed = true;
      }
    }
  }
  
  F->dump();
  
  return changed;
}

bool LoadParameters::runOnFunction(Function *F) {
  IRBuilder builder(F);
  builder.setInsertionPoint(F->front()->front());
  
  bool changed = false;
  
  unsigned index = 1;
  for (Parameter *p : F->getParameters()) {
    auto *load =
        builder.createLoadParamInst(builder.getLiteralNumber(index));
    p->replaceAllUsesWith(load);
    index++;
    changed = true;
  }
  
  return changed;
}
