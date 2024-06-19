/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef File_h
#define File_h

#include <string>

namespace cobra {

class File {
public:
  static constexpr size_t kMagicSize = 4;
  static constexpr size_t kVersionSize = 4;
  
  using Magic = std::array<uint8_t, 8>;
  
  struct Header {
    Magic magic_ = {};
    uint32_t checksum;
    std::array<uint8_t, kVersionSize> version;
    uint32_t fileSize;
    uint32_t numClasses;
    uint32_t classIdxOff;
  };
  
  
};

}

#endif /* File_h */
