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

include(cmake/CompilerFlags.cmake)
include(cmake/ClangTidy.cmake)

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

target_link_libraries(bosce
  PUBLIC
    Qt5::Core
    ${Boost_LIBRARIES}
)
