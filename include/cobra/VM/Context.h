//
//  Context.hpp
//  cobra
//
//  Created by Roy Cao on 2023/12/13.
//

#ifndef Context_hpp
#define Context_hpp

#include "cobra/Support/Allocator.h"

namespace cobra {

class Context {
  
public:
  explicit Context() = default;
  ~Context() = default;
    
  Allocator &getAllocator() {
    return allocator_;
  }
  
  template <typename T>
  T *allocateNode(size_t num = 1) {
    return allocator_.template Allocate<T>(num);
  }
  
  void *allocateNode(size_t size, size_t alignment) {
    return allocator_.Allocate(size, alignment);
  }
  
private:
  Allocator allocator_{};
  
};

}

#endif /* Context_hpp */
