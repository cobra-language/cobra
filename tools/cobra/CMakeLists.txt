# CCopyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

find_library(LIBREADLINE_FOUND readline)
find_library(LIBTINFO_FOUND tinfo)

if(LIBREADLINE_FOUND)
  set(LIBREADLINE ${LIBREADLINE_FOUND})
endif()
if(LIBTINFO_FOUND)
  list(APPEND LIBREADLINE ${LIBTINFO_FOUND})
endif()
set(CMAKE_REQUIRED_LIBRARIES ${LIBREADLINE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_cobra_tool(cobra
  cobra.cpp
  ${ALL_HEADER_FILES}
  )

if(HAVE_LIBREADLINE)
  target_compile_definitions(cobra PRIVATE HAVE_LIBREADLINE)
  set_target_properties(cobra PROPERTIES UNITY_BUILD false)
endif()

target_link_libraries(cobra
  cobraAST
  cobraParser
  ${LIBREADLINE}
)

install(TARGETS cobra
  RUNTIME DESTINATION bin
)
