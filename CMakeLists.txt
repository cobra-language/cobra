# Copyright (c) the Cobra project authors.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

cmake_minimum_required(VERSION 3.13.0)

# Set the VERSION variables based on the project command
if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

# find_package uses <PackageName>_ROOT variables.
if (POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()
# Include file check macros honor CMAKE_REQUIRED_LIBRARIES.
if (POLICY CMP0075)
  cmake_policy(SET CMP0075 NEW)
endif()

# Only interpret if() arguments as variables or keywords when unquoted.
# CMake emits a warning if this is not set.
if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

# Pick up a workaround for a CMake problem from LLVM r282552.
if(POLICY CMP0057)
  cmake_policy(SET CMP0057 NEW)
endif()

# Enable transitive library dependencies
if(POLICY CMP0022)
  cmake_policy(SET CMP0022 NEW)
endif()

# Allow reading the LOCATION property of a target to determine the eventual
# location of build targets. This is needed when building the debugging symbols
# bundles for Apple platforms.
if (POLICY CMP0026)
  cmake_policy(SET CMP0026 OLD)
endif()

# Has to be set before `project` as per documentation
# https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_SYSROOT.html
set(CMAKE_OSX_SYSROOT ${HERMES_APPLE_TARGET_PLATFORM})

if(HERMES_APPLE_TARGET_PLATFORM MATCHES "catalyst")
  set(CMAKE_OSX_SYSROOT "macosx")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-target arm64-apple-ios14.0-macabi -isystem ${CMAKE_OSX_SYSROOT}/System/iOSSupport/usr/include")
  set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-target arm64-apple-ios14.0-macabi -isystem ${CMAKE_OSX_SYSROOT}/System/iOSSupport/usr/include")
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
  set(CMAKE_HAVE_THREADS_LIBRARY 1)
  set(CMAKE_USE_WIN32_THREADS_INIT 0)
  set(CMAKE_USE_PTHREADS_INIT 1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
endif()

# This must be consistent with the release_version in:
# - android/build.gradle
# - npm/package.json
# - hermes-engine.podspec
project(Cobra
        VERSION 0.12.0
        LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/")

include(Cobra)

# This is not a cache variable so that it is recomputed when the project
# version is updated.
if(NOT DEFINED COBRA_RELEASE_VERSION)
  set(COBRA_RELEASE_VERSION ${PROJECT_VERSION})
endif()

find_package(Python COMPONENTS Interpreter)
if (NOT Python_Interpreter_FOUND)
  message(FATAL_ERROR "Unable to find Python interpreter, required for builds and testing.
Please install Python or specify the PYTHON_EXECUTABLE CMake variable.")
endif()

# Project options.

set(COBRA_IS_ANDROID OFF CACHE BOOL
  "Building for Android")

set(HERMES_ENABLE_TOOLS ON CACHE BOOL
  "Enable CLI tools")

CHECK_CXX_SOURCE_COMPILES(
        "int main() { void *p = &&label; goto *p; label: return 0; }"
        HAVE_COMPUTED_GOTO)
if(HAVE_COMPUTED_GOTO)
    set(DEFAULT_INTERPRETER_THREADING ON)
else()
    set(DEFAULT_INTERPRETER_THREADING OFF)
endif()


if(APPLE)
    set(DEFAULT_CONTIGUOUS_HEAP OFF)
else()
    set(DEFAULT_CONTIGUOUS_HEAP ON)
endif()

# On CentOS:
#   sudo yum install zlib-static glibc-static ncurses-static readline-static
set(COBRA_STATIC_LINK OFF CACHE BOOL
  "Link COBRA_STATIC_LINK statically. May only work on GNU/Linux.")

set(HERMES_ENABLE_TEST_SUITE ON CACHE BOOL
  "Enable the test suite")

set(COBRA__BUILD_APPLE_DSYM OFF CACHE BOOL
  "Whether to build a DWARF debugging symbols bundle")

if(COBRA__BUILD_APPLE_DSYM)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf")
endif()


# Enable debug mode by default
if ((NOT GENERATOR_IS_MULTI_CONFIG) AND CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (COBRA_STATIC_LINK)
  append("-static" CMAKE_EXE_LINKER_FLAGS)
  set(HERMES_USE_STATIC_ICU ON)
  set(CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()

# Check if the linker supports --gc-sections and ICF.
# We can't simply CHECK_CXX_COMPILER_FLAG("-Wl,--gc-sections" ..) because CMake
# will compile and link separately and only passes the flag during compilation.
# TODO: Use check_linker_flag once we have CMake 3.18.
set(OLD_CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${OLD_CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
CHECK_CXX_COMPILER_FLAG("" HAVE_GC_SECTIONS)
set(CMAKE_EXE_LINKER_FLAGS "${OLD_CMAKE_EXE_LINKER_FLAGS} -Wl,--icf=safe")
CHECK_CXX_COMPILER_FLAG("" HAVE_ICF)
set(CMAKE_EXE_LINKER_FLAGS "${OLD_CMAKE_EXE_LINKER_FLAGS}")

if(HAVE_GC_SECTIONS)
  add_flag_if_supported("-ffunction-sections" FUNCTION_SECTIONS)
  add_flag_if_supported("-fdata-sections" DATA_SECTIONS)
  list(APPEND COBRA_EXTRA_LINKER_FLAGS "LINKER:--gc-sections")
endif()

if(HAVE_ICF)
  add_flag_if_supported("-faddrsig" ADDRSIG)
  list(APPEND COBRA_EXTRA_LINKER_FLAGS "LINKER:--icf=safe")
endif()

# Make the COBRA_RELEASE_VERSION accessible for version printing in C++.
if(COBRA_RELEASE_VERSION)
    add_definitions(-DHERMES_RELEASE_VERSION="${HERMES_RELEASE_VERSION}")
endif()

if(COBRA_ENABLE_IR_INSTRUMENTATION)
    add_definitions(-DHERMES_ENABLE_IR_INSTRUMENTATION)
endif()


# Collect all header files and add them to the IDE.
file(GLOB_RECURSE ALL_HEADER_FILES "*.h")


if (GCC_COMPATIBLE)
  # Don't export symbols unless we explicitly say so
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
  # C4068 unknown pragma
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4068")
  # C4200 nonstandard extension used: zero-sized array in struct/union
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4200")
  # C4201 nonstandard extension used: nameless struct/union
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4201")
  # C4530 C++ exception handler used, but unwind semantics are not enabled
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -wd4530")
  # Parallelize build
  if (HERMES_MSVC_MP)
    add_definitions( /MP )
  endif()
endif()

# Export a JSON file with the compilation commands that external tools can use
# to analyze the source code of the project.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Disables the GCC 64-to-32 bit truncation diagnostic if the compiler supports
# -Wshorten-64-to-32.
check_cxx_compiler_flag("-Wshorten-64-to-32" CXX_SUPPORTS_SHORTEN_64_TO_32)
if (${CXX_SUPPORTS_SHORTEN_64_TO_32})
  add_definitions(-DHERMES_COMPILER_SUPPORTS_WSHORTEN_64_TO_32)
endif()

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")

include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib)

if(HERMES_ENABLE_TOOLS)
  add_subdirectory(tools)
endif()

# Make sure JSI is compiled with PIC
set(save_CMAKE_POSITION_INDEPENDENT_CODE ${CMAKE_POSITION_INDEPENDENT_CODE})
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(save_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS ${HERMES_BUILD_SHARED_JSI})
set(BUILD_SHARED_LIBS ${save_BUILD_SHARED_LIBS})
set(CMAKE_POSITION_INDEPENDENT_CODE ${save_CMAKE_POSITION_INDEPENDENT_CODE})

# Configure the test suites
#
if(HERMES_ENABLE_TEST_SUITE)
  if(NOT HERMES_ENABLE_TOOLS)
    message(FATAL_ERROR, "Running the test-suite requires the CLI tools to be built.")
  endif()

  list(APPEND COBRA_TEST_DEPS
    hermes
   )


  set(HERMES_LIT_TEST_PARAMS_BASE
    hermes=${COBRA_TOOLS_OUTPUT_DIR}/hermes
    )
endif()
