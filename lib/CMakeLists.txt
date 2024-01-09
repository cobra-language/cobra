# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraFrontend
  IRGen/IRGen.cpp
  IR/IR.cpp
  IR/IRBuilder.cpp
  IR/Instrs.cpp
  LINK_LIBS cobraSupport cobraAST cobraParser
)

add_subdirectory(VM)
add_subdirectory(AST)
add_subdirectory(Parser)
add_subdirectory(Support)
