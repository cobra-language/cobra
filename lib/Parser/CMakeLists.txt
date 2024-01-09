# CCopyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraParser
  Lexer.cpp
  Parser.cpp
  LINK_LIBS cobraSupport cobraAST
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
