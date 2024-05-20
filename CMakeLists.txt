cmake_minimum_required(VERSION 3.0.0)
project(huffman VERSION 0.1.0 LANGUAGES CXX)
set (CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

add_executable(huffman main.cpp HuffmanTree.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
