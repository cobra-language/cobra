/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/CotFile.h"

using namespace cobra;

uint32_t CotFile::Header::getVersion() const {
  const char* version = reinterpret_cast<const char*>(&magic_[kMagicSize]);
  return atoi(version);
}

inline const char *CotFile::getStringData(EntityId id) const {
  auto array = getArrayFromId(id);
  return reinterpret_cast<const char*>(array.data());
}

static ArraySlice<const uint8_t> getData(const uint8_t *base) {
  auto header = reinterpret_cast<const CotFile::Header*>(base);
  ArraySlice array(base, header->fileSize);
  return array;
}

CotFile::CotFile(const uint8_t *base, std::string location)
    : location_(location),
      header_(reinterpret_cast<const Header*>(base)),
      stringIds_(getSection<EntityId>(header_->stringIdxOffset)),
      fieldIds_(getSection<EntityId>(header_->fieldIdxOffset)),
      methodIds_(getSection<EntityId>(header_->methodIdxOffset)),
      protoIds_(getSection<EntityId>(header_->protoIdxOffset)),
      data_(getData(base)) {
        
}

template <typename T>
inline const T *CotFile::getSection(const uint32_t offset) {
  return reinterpret_cast<const T*>(data_.data() + offset);
}

std::unique_ptr<const CotFile> openBytecodeFile(std::string_view location) {
  uint32_t magic;

  FILE *fp = fopen(std::string(location).c_str(), "rb");
  if (fp == nullptr) {
      return nullptr;
  }
  (void)fseek(fp, 0, SEEK_SET);
  if (fread(&magic, sizeof(magic), 1, fp) != 1) {
      fclose(fp);
      return nullptr;
  }
  (void)fseek(fp, 0, SEEK_SET);
  std::unique_ptr<const CotFile> file;
  
  fclose(fp);
}
