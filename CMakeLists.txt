cmake_minimum_required(VERSION 3.29)
project(QuadTreeProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#Flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdouble-promotion -Werror -Wall -Wextra -ffast-math -O3 -Ofast -march=native -mtune=native -funroll-loops -finline-functions -finline-limit=1000 -fno-rtti -fno-exceptions -fno-stack-protector -fomit-frame-pointer -fstack-protector -fstack-protector-all -fstack-protector-strong ")
enable_testing()

find_package(GTest REQUIRED)

add_library(QuadTree
        QuadTree/QuadTree.cpp
        QuadTree/QuadTree.hpp
        QuadTree/QuadTree.tpp
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

