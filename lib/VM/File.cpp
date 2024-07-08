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

