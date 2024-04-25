# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraOptimizer
  CSE.cpp
  DCE.cpp
  Inlining.cpp
  Mem2Reg.cpp
  PassManager.cpp
  Pipeline.cpp
  SimplifyCFG.cpp
  PhiElimination.cpp
  LINK_LIBS cobraFrontend
)
