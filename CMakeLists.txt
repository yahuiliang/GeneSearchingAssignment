cmake_minimum_required(VERSION 3.10)

project(ProgrammingProject)

SET(CMAKE_CXX_STANDARD 14)

include_directories(ProgrammingProject)
include_directories(Test)
include_directories(libs/include)
link_directories(libs/bin)

add_executable(
        ProgrammingProject
        ProgrammingProject/main.cpp
        ProgrammingProject/Sequence.cpp
        ProgrammingProject/DnaABoxesGenerator.cpp)

add_executable(
        Test
        Test/main.cpp
        ProgrammingProject/Sequence.cpp
        ProgrammingProject/DnaABoxesGenerator.cpp)

enable_testing()
add_test(NAME SequenceTests COMMAND Test)

target_link_libraries(ProgrammingProject restclient-cpp curl)
