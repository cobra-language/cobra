# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraVMRuntime
  CBMethod.cpp
  Object.cpp
  CBString.cpp
  CBValue.cpp
  CodeBlock.cpp
  GC.cpp
  GCCell.cpp
  Interpreter.cpp
  Primitive.cpp
  Runtime.cpp
  RuntimeModule.cpp
  String.cpp
  VM.cpp
  CardTable.cpp
  HeapRegion.cpp
  Handle.cpp
  Offset.cpp
  GCPointer.cpp
  LINK_LIBS cobraSupport
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
