##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

cmake_minimum_required(VERSION 3.10)
project(bosce VERSION 1.3 LANGUAGES CXX)

enable_testing()

find_package(Boost REQUIRED COMPONENTS program_options filesystem)
find_package(Threads REQUIRED)

include(GNUInstallDirs)
include(cmake/AddressSanitizer.cmake)
include(cmake/ClangTidy.cmake)
include(cmake/CompilerFlags.cmake)

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
    src/YamlGenerator.cpp
)

target_link_libraries(bosce
  PUBLIC
    Threads::Threads
    Boost::program_options
    Boost::filesystem
)

install(
  TARGETS
    bosce
  RUNTIME DESTINATION
    ${CMAKE_INSTALL_BINDIR}
)

add_subdirectory(test)
