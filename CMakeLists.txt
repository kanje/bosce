##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

cmake_minimum_required(VERSION 3.3)
project(bosce CXX)

find_package(Qt5 REQUIRED COMPONENTS Core CONFIG)
find_package(Boost REQUIRED COMPONENTS program_options)

# Compile with these options:
add_compile_options(
    -pedantic
    -Werror
    -Wall
    -Wextra
)

# Use clang-tidy:
find_program(CLANG_TIDY_EXECUTABLE
  NAMES
    clang-tidy
)
if(CLANG_TIDY_EXECUTABLE)
    set(CMAKE_CXX_CLANG_TIDY
        ${CLANG_TIDY_EXECUTABLE}
        -checks=-*,clang*,performance*,readability*,misc*,modernize*,-readability-implicit-bool-cast
    )
endif()

add_executable(bosce
    src/main.cpp
    src/ScModel.cpp
    src/ScParser.cpp
    src/ObjDumpParser.cpp
    src/ParserHelpers.h
    src/AbstractGenerator.h
    src/StmListGenerator.cpp
    src/TextGenerator.cpp
    src/PlantUmlGenerator.cpp
)

target_compile_features(bosce
  PUBLIC
    cxx_std_14 # enforce C++14
)

target_link_libraries(bosce
  PUBLIC
    Qt5::Core
    ${Boost_LIBRARIES}
)

# Add these files to IDE:
add_custom_target(extra-files
  SOURCES
    README.md
    LICENSE
)
