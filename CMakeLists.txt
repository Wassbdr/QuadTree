cmake_minimum_required(VERSION 3.29)
project(QuadTreeProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
enable_testing()

find_package(GTest REQUIRED)

add_library(QuadTree
        QuadTree/QuadTree.cpp
        QuadTree/QuadTree.hpp
)

add_executable(QuadTreeTest
        QuadTree/QuadTreeTest.cpp
)
target_link_libraries(QuadTreeTest
        PRIVATE
        GTest::GTest
        GTest::Main
        QuadTree
)

add_executable(QuadTreeMain main.cpp)
target_link_libraries(QuadTreeMain QuadTree)

add_test(NAME QuadTreeTest COMMAND QuadTreeTest)

