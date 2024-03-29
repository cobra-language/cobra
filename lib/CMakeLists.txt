# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraFrontend
  IRGen/IRGen.cpp
  IR/IR.cpp
  IR/IRBuilder.cpp
  IR/Instrs.cpp
  IR/Dominance.cpp
  IR/Analysis.cpp
  Utils/IRPrinter.cpp
  IRGen/IRGenStmt.cpp
  IRGen/IRGenExpr.cpp
  IRGen/IRGenFunction.cpp
  LINK_LIBS cobraSupport cobraAST cobraParser
)

add_subdirectory(VM)
add_subdirectory(AST)
add_subdirectory(Parser)
add_subdirectory(Support)
add_subdirectory(BCGen)
add_subdirectory(Optimizer)
add_subdirectory(Driver)
