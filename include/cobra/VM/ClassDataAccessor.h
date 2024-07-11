/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ClassDataAccessor_h
#define ClassDataAccessor_h

#include <string>

#include "cobra/VM/CotFile.h"

namespace cobra {

/// Ref ArkCompiler ClassDataAccessor
/// And Art ClassDataAccessor
class ClassDataAccessor {
public:
  ClassDataAccessor(const CotFile &file, uint32_t classID);
  
  ~ClassDataAccessor() = default;
  
  uint32_t getStaticFieldCount() const {
    return staticFieldCount_;
  }
  
  uint32_t getInstanceFieldCount() const {
    return instanceFieldCount_;
  }
  
  uint32_t getFieldCount() const {
    return staticFieldCount_ + instanceFieldCount_;
  }
  
  uint32_t getDirectMethodCount() const {
    return directMethodCount_;
  }
  
  uint32_t getVirtualMethodCount() const {
    return directMethodCount_;
  }
  
  uint32_t getMethodCount() const {
    return directMethodCount_ + virtualMethodCount_;
  }
  
  const CotFile &getFile() const {
    return file_;
  }
  
  const char *getDescriptor() const;
  
private:
  const CotFile &file_;
  uint32_t classID_;
  uint32_t access_flags_;
  uint32_t staticFieldCount_{0};
  uint32_t instanceFieldCount_{0};
  uint32_t directMethodCount_{0};
  uint32_t virtualMethodCount_{0};
  
};

}

#endif /* ClassDataAccessor_h */
