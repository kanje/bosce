##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

option(WITH_CLANG_TIDY "Build with clang-tidy")

if(NOT WITH_CLANG_TIDY)
    return()
endif()

find_program(CLANG_TIDY_EXECUTABLE
  NAMES
    clang-tidy
)

if(NOT CLANG_TIDY_EXECUTABLE)
    message(FATAL_ERROR "Cannot find clang-tidy!")
endif()

set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
