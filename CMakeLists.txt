cmake_minimum_required(VERSION 3.3)
project(bosce)

find_package(Qt5 QUIET REQUIRED Core CONFIG NO_CMAKE_PACKAGE_REGISTRY)

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
)
