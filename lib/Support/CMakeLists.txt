# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraSupport
  Base64.cpp
  StringRef.cpp
  OSCompatPosix.cpp
  zip.cpp
  LINK_LIBS ${link_libs}
)
