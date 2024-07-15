/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef CotFile_h
#define CotFile_h

#include <string>
#include "cobra/Support/ArraySlice.h"
#include "cobra/Support/zip.h"

namespace cobra {

const static uint8_t StandardFileMagic[] = { 'c', 'o', 'b', 'r', 'a', '\n' };

class CotFile;

class ClassIdxIterator {
public:
  using value_type = const uint8_t *;
  using difference_type = std::ptrdiff_t;
  using pointer = uint32_t *;
  using reference = uint32_t &;
  using iterator_category = std::random_access_iterator_tag;
  
  ClassIdxIterator(const CotFile &file, size_t idx) : file_(file), idx_(idx) {}
  
  ClassIdxIterator(const ClassIdxIterator &other) = default;
  ClassIdxIterator(ClassIdxIterator &&other) = default;
  ~ClassIdxIterator() = default;
  
  ClassIdxIterator &operator++() {
    ++idx_;
    return *this;
  }
  
  ClassIdxIterator &operator--() {
    --idx_;
    return *this;
  }
  
private:
  const CotFile &file_;
  size_t idx_;
};

/// Cot file is the bytecode  binary format file of cobra
/// Ref to ArkCompiler panda_file
/// and art DexFile
class CotFile {
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
    uint32_t stringIdxCount;  // number of StringIds
    uint32_t stringIdxOffset;  // file offset of StringIds array
    uint32_t classIdxCount;  // number of Class
    uint32_t classIdxOffset;  // file offset of ClassDef array
    uint32_t methodIdxCount;  // number of MethodIds
    uint32_t methodIdxOffset;  // file offset of MethodIds array
    uint32_t fieldIdxCount;  // number of FieldIds
    uint32_t fieldIdxOffset;  // file offset of FieldIds array
    uint32_t protoIdxCount;  // number of ProtoIds, we don't support more than 65535
    uint32_t protoIdxOffset;  // file offset of ProtoIds array
    uint32_t dataSize;  // size of data section
    uint32_t dataOffset;  // file offset of data section
    
    // decode the magic version
    uint32_t getVersion() const;
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
    return data_.data();
  }
  
  uint32_t getVersion() const {
    return header_->getVersion();
  }
  
  bool isMagicValid() const {
    return ::memcmp(header_->magic_.data(), StandardFileMagic, sizeof(StandardFileMagic)) == 0;
  }
  
  size_t size() const { return header_->fileSize; }
  
  const char *getStringData(EntityId id) const;
  
  ArraySlice<const uint8_t> getArrayFromId(EntityId id) const {
    ArraySlice array(getBase(), header_->fileSize);
    return array.last(array.size() - id.getOffset());
  }
  
  ArraySlice<const uint32_t> getClasses() const {
    ArraySlice file(getBase(), header_->fileSize);
    ArraySlice classIdxData = file.subArray(header_->classIdxOffset, header_->classIdxCount * sizeof(uint32_t));
    return ArraySlice(reinterpret_cast<const uint32_t *>(classIdxData.data()), header_->classIdxCount);
  }
  
  size_t stringIdxCount() const {
    return header_->stringIdxCount;
  }
  
  const EntityId getStringId(uint32_t idx) const {
    return stringIds_[idx];
  }
  
  size_t methodIdxCount() const {
    return header_->methodIdxCount;
  }
  
  const EntityId getMethodId(uint32_t idx) const {
    return methodIds_[idx];
  }
  
  size_t fieldIdxCount() const {
    return header_->fieldIdxCount;
  }
  
  const EntityId getFieldId(uint32_t idx) const {
    return fieldIds_[idx];
  }
  
  size_t protoIdxCount() const {
    return header_->protoIdxCount;
  }
  
  const EntityId getProtoId(uint32_t idx) const {
    return protoIds_[idx];
  }
  
  template <typename T>
  const T *getSection(const uint32_t offset);
  
  static std::unique_ptr<const CotFile> open(std::string_view filename);
  
private:
  /// The full absolute path to the dex file.
  const std::string location_;
  
  const Header *const header_;
  
  ArraySlice<const uint8_t> const data_;
  
  const EntityId *const stringIds_;
  
  const EntityId *const fieldIds_;
  
  const EntityId *const methodIds_;
  
  const EntityId *const protoIds_;
  
  CotFile(const uint8_t *base, std::string location);
  
};

std::unique_ptr<const CotFile> openCotFile(std::string_view location);

}

#endif /* CotFile_h */
