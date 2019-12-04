##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

option(WITH_WARNINGS "Build with compiler warning enabled")

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 14)

if(WITH_WARNINGS)
    add_compile_options(
        -pedantic
        -Werror
        -Wall
        -Wextra
    )
endif()
