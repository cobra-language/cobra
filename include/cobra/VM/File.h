/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef File_h
#define File_h

#include <string>
#include "cobra/Support/ArraySlice.h"

namespace cobra {

const static uint8_t StandardFileMagic[] = { 'c', 'o', 'b', 'r', 'a', '\n' };

class File {
public:
  static constexpr size_t kMagicSize = 4;
  static constexpr size_t kVersionSize = 4;
  static constexpr size_t kSha1DigestSize = 20;
  using Sha1 = std::array<uint8_t, kSha1DigestSize>;
  
  using Magic = std::array<uint8_t, 8>;
  
  struct Header {
    Magic magic_ = {};
    uint32_t checksum;
    std::array<uint8_t, kVersionSize> version;
    uint8_t sourceHash[kSha1DigestSize];
    uint32_t fileSize;
    uint32_t classCount;
    uint32_t classIdxOffset;
    
    // decode the magic version
    uint32_t getVersion() const;
  };
  
  struct RegionHeader {
    uint32_t start;
    uint32_t end;
    uint32_t classIdxSize;  // number of Class
    uint32_t classIdxOffset;  // file offset of ClassDef array
    uint32_t methodIdxSize;  // number of MethodIds
    uint32_t methodIdxOffset;  // file offset of MethodIds array
    uint32_t fieldIdxSize;  // number of FieldIds
    uint32_t fieldIdxOffset;  // file offset of FieldIds array
    uint32_t protoIdxSize;  // number of ProtoIds, we don't support more than 65535
    uint32_t protoIdxOffset;  // file offset of ProtoIds array
  };
  
  class EntityId {
  public:
    explicit constexpr EntityId(uint32_t offset) : offset_(offset) {}

    EntityId() = default;

    ~EntityId() = default;

    bool isValid() const {
      return offset_ > sizeof(Header);
    }

    uint32_t getOffset() const {
      return offset_;
    }

    static constexpr size_t getSize() {
      return sizeof(uint32_t);
    }

    friend bool operator<(const EntityId &l, const EntityId &r) {
      return l.offset_ < r.offset_;
    }

    friend bool operator==(const EntityId &l, const EntityId &r) {
      return l.offset_ == r.offset_;
    }

  private:
    uint32_t offset_ {0};
  };
  
public:
  const Header &getHeader() const {
    return *header_;
  }
  
  const uint8_t *getBase() const {
    // TODO
  }
  
  uint32_t getVersion() const {
    return getHeader().getVersion();
  }
  
  bool isMagicValid() const {
    return ::memcmp(header_->magic_.data(), StandardFileMagic, sizeof(StandardFileMagic)) == 0;
  }
  
  const uint8_t *dataBegin() const { return data_.data(); }

  size_t dataSize() const { return data_.size(); }
  
  const char *getStringData(EntityId id) const;
  
  ArraySlice<const uint8_t> getArrayFromId(EntityId id) const {
    const Header header = getHeader();
    ArraySlice array(dataBegin(), header.fileSize);
    return array.last(array.size() - id.getOffset());
  }
  
private:
  /// The full absolute path to the dex file.
  const std::string location_;
  
  const Header* const header_;
  
  const uint8_t *bufferPtr_;
  
  ArraySlice<const uint8_t> const data_;
  
};

}

#endif /* File_h */
