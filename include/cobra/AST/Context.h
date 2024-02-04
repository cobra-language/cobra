/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Context_hpp
#define Context_hpp

#include "cobra/Support/Allocator.h"
#include "cobra/Support/StringTable.h"

namespace cobra {

class Context {
private:
  Allocator allocator_{};
  Allocator identifierAllocator_{};
  StringTable stringTable_{identifierAllocator_};
  
public:
  explicit Context() = default;
  ~Context() = default;
    
  Allocator &getAllocator() {
    return allocator_;
  }
  
  Identifier getIdentifier(StringRef str) {
    return stringTable_.getIdentifier(str);
  }
  
  StringRef toString(Identifier iden) {
    return iden.str();
  }
  
  template <typename T>
  T *allocateNode(size_t num = 1) {
    return allocator_.template Allocate<T>(num);
  }
  
  void *allocateNode(size_t size, size_t alignment) {
    return allocator_.Allocate(size, alignment);
  }
  
};

}

#endif /* Context_hpp */
