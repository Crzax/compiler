cmake_minimum_required(VERSION 3.26)
project(lexSynAnalysis)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_executable(LexAndParser src/Lex.cpp src/Parser.cpp src/main.cpp)