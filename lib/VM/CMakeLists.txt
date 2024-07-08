# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

add_cobra_library(cobraVMRuntime
  Method.cpp
  Object.cpp
  String.cpp
  Class.cpp
  Field.cpp
  Array.cpp
  InterfaceTable.cpp
  CBValue.cpp
  CodeBlock.cpp
  GC.cpp
  GCRoot.cpp
  GCCell.cpp
  Interpreter.cpp
  Primitive.cpp
  Runtime.cpp
  RuntimeModule.cpp
  String.cpp
  CardTable.cpp
  HeapRegion.cpp
  Handle.cpp
  Offset.cpp
  GCPointer.cpp
  ClassLinker.cpp
  RuntimeOptions.cpp
  CobraVM.cpp
  File.cpp
  StackFrame.cpp
  ObjectAccessor.cpp
  FreeList.cpp
  MemMapAllocator.cpp
  HeapRegionSpace.cpp
  CodeDataAccessor.cpp
  CobraCache.cpp
  LINK_LIBS cobraSupport
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
