cmake_minimum_required(VERSION 3.22)
project(CalcGTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR})

add_library(CalcLib
    SimpleCalculator.cpp
    InMemoryHistory.cpp
)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_library(GMOCK_LIBRARY NAMES gmock PATHS /usr/lib)
find_library(GMOCK_MAIN_LIBRARY NAMES gmock_main PATHS /usr/lib)
find_path(GMOCK_INCLUDE_DIR NAMES gmock/gmock.h PATHS /usr/include)
include_directories(${GMOCK_INCLUDE_DIR})

add_executable(runTests
    SimpleCalculatorTest.cpp
)

target_link_libraries(runTests
    CalcLib
    ${GTEST_LIBRARIES}
    ${GTEST_MAIN_LIBRARIES}
    ${GMOCK_LIBRARY}
    ${GMOCK_MAIN_LIBRARY}
    pthread
)

enable_testing()
add_test(NAME CalculatorTests COMMAND runTests)

option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lgcov")
endif()