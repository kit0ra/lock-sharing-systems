cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS "-g -D COLOURING -D DEADLOCK_CHECKING")

project(graphProblemSolver C)

set(CMAKE_VERBOSE_MAKEFILE OFF)

include_directories(include/main include/ColouringProblem src/parser/include include/BoundedDeadlockChecking)

file(GLOB SOURCES examples/*.c src/*/*.c src/parser/Lexer.l src/parser/Parser.y parser src/parser/src/*.c)

add_library(myGraph src/main/Graph.c)
add_library(myZ3 src/main/Z3Tools.c)

find_package(FLEX)
find_package(BISON)

if(FLEX_FOUND)
if(BISON_FOUND)
flex_target(MyLexer src/parser/Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/Lexer.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Lexer.h)
bison_target(MyParser src/parser/Parser.y ${CMAKE_CURRENT_BINARY_DIR}/Parser.c DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/Parser.h)
add_flex_bison_dependency(MyLexer MyParser)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


add_library(parser src/parser/src/EdgeList.c src/parser/src/NodeList.c src/parser/src/GraphListToGraph.c src/parser/src/Parsing.c ${BISON_MyParser_OUTPUTS} ${FLEX_MyLexer_OUTPUTS})

file(GLOB ColourFiles src/ColouringProblem/*.c)
add_library(colouringPb ${ColourFiles})
file(GLOB DeadlockFiles src/BoundedDeadlockChecking/*.c)
add_library(deadlockPb ${DeadlockFiles})

add_executable(graphProblemSolver src/main/main.c)
target_link_libraries(graphProblemSolver z3 myGraph myZ3 parser colouringPb deadlockPb)


add_executable(graphParser examples/graphUsage.c)
target_link_libraries(graphParser myGraph parser deadlockPb)

endif(BISON_FOUND)
endif(FLEX_FOUND)

add_executable(Z3Example examples/Z3Example.c)
target_link_libraries(Z3Example z3 myZ3)
