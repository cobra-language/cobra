/*
 * Copyright (c) the Cobra project authors.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "cobra/VM/File.h"

using namespace cobra;

uint32_t File::Header::getVersion() const {
  const char* version = reinterpret_cast<const char*>(&magic_[kMagicSize]);
  return atoi(version);
}

inline const char *File::getStringData(EntityId id) const {
  auto array = getArrayFromId(id);
  return reinterpret_cast<const char*>(array.data());
}

static ArraySlice<const uint8_t> getData(const uint8_t *base) {
  auto header = reinterpret_cast<const File::Header*>(base);
  ArraySlice array(base, header->fileSize);
  return array;
}

File::File(const uint8_t *base, std::string location)
    : location_(location),
      header_(reinterpret_cast<const Header*>(base)),
      data_(getData(base)) {

}

template <typename T>
inline const T *File::getSection(const uint32_t offset) {
  return reinterpret_cast<const T*>(data_.data() + offset);
}

std::unique_ptr<const File> openBytecodeFile(std::string_view location) {
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
  std::unique_ptr<const File> file;
  
  fclose(fp);
}
