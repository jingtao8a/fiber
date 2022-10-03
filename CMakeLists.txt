cmake_minimum_required(VERSION 3.18)
project(fiber)

add_executable(testucontext 
            tests/testucontext.cpp
            src/uthread.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)