/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef Allocator_h
#define Allocator_h

#include <stdio.h>
#include <cstddef>

namespace cobra {

class Allocator {
public:
  
  explicit Allocator()
    : root(static_cast<Page*>(operator new(sizeof(Page)))) , offset(0) {
    root->next = nullptr;
  }
  
  ~Allocator() {
    Page* page = root;
    
    while (page) {
      Page* next = page->next;
      operator delete(page);
      page = next;
    }
  }

  inline void *Allocate(
      size_t size,
      size_t alignment = kDefaultPlatformAlignment) {
    if (root) {
      uintptr_t data = reinterpret_cast<uintptr_t>(root->data);
      uintptr_t result = (data + offset + alignment - 1) & ~(alignment - 1);
      if (result + size <= data + sizeof(root->data)) {
        offset = result - data + size;
        return reinterpret_cast<void*>(result);
      }
    }
    
    // allocate new page
    size_t pageSize = size > sizeof(root->data) ? size : sizeof(root->data);
    void *pageData = operator new(offsetof(Page, data) + pageSize);
    
    Page* page = static_cast<Page*>(pageData);
    page->next = root;
    
    root = page;
    offset = size;
    
    return page->data;
  }
  
  template <typename T>
  inline T *Allocate(size_t num = 1, size_t alignment = sizeof(double)) {
    int size = sizeof(T) * num;
    return static_cast<T *>(Allocate(size, alignment));
  }

private:
  
  static const unsigned kDefaultPlatformAlignment = sizeof(double);
  
  struct Page {
    Page* next;
    char data[8192];
  };

  Page* root;
  size_t offset;
};

}

#endif /* Allocator_h */
