##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

set(CMAKE_CXX_FLAGS_ASAN
    "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined")
set(CMAKE_EXE_LINKER_FLAGS_ASAN "${CMAKE_EXE_LINKER_FLAGS}")
set(CMAKE_SHARED_LINKER_FLAGS_ASAN "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
set(CMAKE_STATIC_LINKER_FLAGS_ASAN "${CMAKE_STATIC_LINKER_FLAGS_DEBUG}")
