/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CodeDataAccessor_h
#define CodeDataAccessor_h

#include <string>

#include "cobra/VM/CexFile.h"

namespace cobra {

/// Ref ArkCompiler CodeDataAccessor
/// And Art CodeItemDataAccessor
class CodeDataAccessor {
public:
  
  CodeDataAccessor(const CexFile &file);
  
  ~CodeDataAccessor() = default;
  
  inline static const uint8_t *getInstructions(const CexFile &file);
  
  const uint8_t *getInstructions() const {
    return instructionsPtr_;
  }
  
private:
  const CexFile &file_;
  
  /// Pointer to the instructions, null if there is no code item.
  const uint8_t *instructionsPtr_;
  
  /// the number of try_items for this instance. If non-zero,
  /// then these appear as the tries array just after the
  /// insns in this instance.
  uint32_t tries_size_;
  
};

const uint8_t *CodeDataAccessor::getInstructions(const CexFile &file) {
  // TODO
}

}

#endif /* CodeDataAccessor_h */
