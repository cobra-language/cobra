# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraBackend
  RegAlloc.cpp
  Bytecode.cpp
  Lowering.cpp
  BytecodeGenerator.cpp
  BCGen.cpp
  BytecodeRawData.cpp
  BytecodeInstructionSelector.cpp
  BCPasses.cpp
  LINK_LIBS cobraFrontend cobraOptimizer
)
