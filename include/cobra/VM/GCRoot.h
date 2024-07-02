/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef GCRoot_h
#define GCRoot_h

#include <string>

#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

enum RootType {
  Unknown = 0,
  Class,
  Frame,
  Local,
  StringTable,
};

class RootVisitor {
public:
  virtual ~RootVisitor() { }
  
  void VisitRoot(Object** root, RootType type) {
    
  }
  
  virtual void VisitRoots(Object*** roots, size_t count, RootType type) {
    
  }
  
};

class GCRoot {
public:
  
  GCRoot() : GCRoot(nullptr) {}
  GCRoot(std::nullptr_t) : root_() {}
  GCRoot(Object *object) : root_(object) {}
  
  Object *getRoot() const {
    return root_;
  }
  
  void VisitRoot(RootVisitor* visitor, RootType type) const {
    
  }
  
private:
  Object *root_;
  
};


}
}

#endif /* GCRoot_h */
