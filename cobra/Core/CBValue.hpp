//
//  CBValue.hpp
//  cobra
//
//  Created by Roy Cao on 2023/11/24.
//

#ifndef CBValue_hpp
#define CBValue_hpp

#include <stdio.h>
#include <map>

namespace cobra {

#define DECLARE_BUFFER(name, type)                                             \
    typedef struct                                                             \
    {                                                                          \
      type* data;                                                              \
      int count;                                                               \
      int capacity;                                                            \
    } name##Buffer;

DECLARE_BUFFER(Uint, uint32_t)
DECLARE_BUFFER(Byte, uint8_t)


}

#endif /* CBValue_hpp */
