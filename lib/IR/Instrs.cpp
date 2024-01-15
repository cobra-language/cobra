/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/IR/Instrs.h"

using namespace cobra;

unsigned TerminatorInst::getNumSuccessors() {
#undef TERMINATOR
#define TERMINATOR(CLASS, PARENT)           \
  if (auto I = dynamic_cast<CLASS *>(this)) \
    return I->getNumSuccessors();
#include "cobra/IR/Instrs.def"
}

//BasicBlock *TerminatorInst::getSuccessor(unsigned idx) const {
//#undef TERMINATOR
//#define TERMINATOR(CLASS, PARENT)           \
//  if (auto I = const_cast<CLASS *>(this)) \
//    return I->getSuccessor(idx);
//#include "cobra/IR/Instrs.def"
//}
//
//void TerminatorInst::setSuccessor(unsigned idx, BasicBlock *B) {
//#undef TERMINATOR
//#define TERMINATOR(CLASS, PARENT)           \
//  if (auto I = const_cast<CLASS *>(this)) \
//    return I->setSuccessor(idx, B);
//#include "cobra/IR/Instrs.def"
//}
