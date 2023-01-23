##
## Boost StateChart Extractor
##
## Distributed under the Boost Software License, Version 1.0.
## (See accompanying file LICENSE or copy at
##                      http://www.boost.org/LICENSE_1_0.txt)
##

configure_file(test_plantuml.sh.in test_plantuml.sh @ONLY)

macro(add_plantuml_test TESTNAME DATANAME STMNAME)
    add_test(
      NAME ${TESTNAME}
      COMMAND test_plantuml.sh ${DATANAME} ${STMNAME}
    )
endmacro()

add_plantuml_test(StopWatch 01 StopWatch)
add_plantuml_test(InitialStateBeforeStm 02 StopWatch)
add_plantuml_test(CustomTransitionToTemplateState 03 SM)
add_plantuml_test(HistoryPseudoState 04 Camera)
add_plantuml_test(OrthogonalHistory 05 finite_state_machine::SmMachine)
add_plantuml_test(NDebug 06 Plinio::OtherStateMachine)

add_custom_target(demo
  COMMAND
    ${CMAKE_CXX_COMPILER} -g ${CMAKE_CURRENT_LIST_DIR}/src/demo/main.cpp -o demo
  WORKING_DIRECTORY
    ${CMAKE_BINARY_DIR}
  SOURCES
    src/demo/main.cpp
  COMMENT
    "Building a demo statechert binary"
)

add_custom_target(shared
  COMMAND
    ${CMAKE_CXX_COMPILER} -g ${CMAKE_CURRENT_LIST_DIR}/src/shared/shared.cpp -fPIC -shared
        -o libshared.so
  COMMAND
    ${CMAKE_CXX_COMPILER} -g ${CMAKE_CURRENT_LIST_DIR}/src/shared/main.cpp -L${CMAKE_BINARY_DIR} -lshared -o shared
  WORKING_DIRECTORY
    ${CMAKE_BINARY_DIR}
  SOURCES
    src/shared/shared.cpp
    src/shared/shared.h
    src/shared/main.cpp
  COMMENT
    "Building a shared statechert binary"
)
