cmake_minimum_required(VERSION 3.26)
project(lexSynAnalysis)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(LexAndParser Lex.cpp Parser.cpp main.cpp)