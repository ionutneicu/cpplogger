cmake_minimum_required(VERSION 3.0.2)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_CXX_STANDARD 11)
include_directories( src )

file(GLOB tests_SRC ./src/*.cpp ./src/*.h )
add_executable(tests ${tests_SRC})
target_link_libraries(tests pthread )


# Locate GTest
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS} src)

# Link runTests with what we want to test and the GTest and pthread library
#add_executable(runTests ${tests_SRC})
#target_link_libraries(runTests ivo ${GTEST_LIBRARIES} pthread )
