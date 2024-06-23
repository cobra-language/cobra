/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef String_h
#define String_h

#include "cobra/VM/Object.h"

namespace cobra {
namespace vm {

// String Compression
static constexpr bool kUseStringCompression = true;
enum class StringCompressionFlag : uint32_t {
  kCompressed = 0u,
  kUncompressed = 1u
};

class String : public Object {
  
public:
  String() = default;
  
  static constexpr uint32_t getLengthOffset() {
    return MEMBER_OFFSET(String, length_);
  }

  static constexpr uint32_t getDataOffset() {
    return MEMBER_OFFSET(String, data_);
  }

  static constexpr uint32_t getHashCodeOffset() {
    return MEMBER_OFFSET(String, hashCode_);
  }
  
  uint16_t *getData() {
    return &data_[0];
  }

  uint8_t *getDataCompressed() {
    return &dataCompressed_[0];
  }
  
  uint32_t getLength() const {
    return kUseStringCompression ? (length_ >> 1U) : length_;
  }
  
  bool isEmpty() const{
    return length_ == 0;
  }
  
  uint32_t getHashCode() {
    if (hashCode_ == 0) {
      hashCode_ = computeHashCode();
    }
    return hashCode_;
  }
  
  uint32_t computeHashCode();
  
  bool isCompressed() {
    return kUseStringCompression && isCompressed(getLength());
  }
  
   static bool isCompressed(int32_t length) {
    return getCompressionFlagFromLength(length) == StringCompressionFlag::kCompressed;
  }

  static StringCompressionFlag getCompressionFlagFromLength(int32_t length) {
    return kUseStringCompression
    ? static_cast<StringCompressionFlag>(length & 1u)
    : StringCompressionFlag::kUncompressed;
  }
    
  bool equals(String* that);
  
private:
  uint32_t length_;
  uint32_t hashCode_;
  
  /// Compression of all-ASCII into 8-bit memory leads to usage one of these fields
  union {
    uint16_t data_[0];
    uint8_t dataCompressed_[0];
  };
};

}
}

#endif /* String_h */
