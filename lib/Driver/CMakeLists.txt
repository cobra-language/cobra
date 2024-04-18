# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

set(source_files
  Driver.cpp
)

add_cobra_library(cobraDriver STATIC ${source_files}
  LINK_LIBS
  cobraAST
  cobraBackend
  cobraOptimizer
  cobraFrontend
  cobraParser
  cobraSupport
  cobraVMRuntime
)
