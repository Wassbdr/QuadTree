cmake_minimum_required(VERSION 3.29)
project(QuadTree)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the QuadTree library
add_library(QuadTree QuadTree/QuadTree.cpp
)

# Add the main executable
add_executable(QuadTreeMain main.cpp)
target_link_libraries(QuadTreeMain QuadTree)

# Add the test executable
add_executable(QuadTreeTest QuadTree/QuadTreeTest.cpp)
target_link_libraries(QuadTreeTest QuadTree)

# Enable testing
enable_testing()
add_test(NAME QuadTreeTest COMMAND QuadTreeTest)
