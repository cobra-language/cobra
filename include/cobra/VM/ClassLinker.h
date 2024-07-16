/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef ClassLinker_h
#define ClassLinker_h

#include "cobra/VM/Class.h"
#include "cobra/VM/CexFile.h"
#include "cobra/VM/ClassDataAccessor.h"

namespace cobra {
namespace vm {

class ClassLinker {
  
public:
  ClassLinker() = default;
  
  ~ClassLinker();
  
  Class *getClass(const uint8_t *descriptor);
  
  Class *loadClass(const CexFile *file, uint32_t classID);
  
  bool loadFields(Class *klass);
  
  bool loadMethods(Class *klass);
  
private:
  
};

}
}

#endif /* ClassLinker_h */
