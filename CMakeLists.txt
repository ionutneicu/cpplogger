cmake_minimum_required(VERSION 3.0.2)
project(cpploger)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_STANDARD 11)

file(GLOB cpplog_SRC ./src/*.cpp ./src/*.h )
add_library(cpplog SHARED ${cpplog_SRC})
target_include_directories( cpplog PUBLIC src )


file(GLOB examples_SRC ./examples/*.cpp ./examples/*.h )
add_executable(examples ${examples_SRC})
target_include_directories( examples PUBLIC examples )
target_link_libraries( examples cpplog )
