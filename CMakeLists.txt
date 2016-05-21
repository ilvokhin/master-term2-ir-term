cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED)

SET(CMAKE_C_FLAGS "-Wall -O3 -std=c++11 -pedantic")


include_directories(
  include
  OleanderStemmingLibrary
)

add_library(common src/common.cpp)

set(SOURCES 
  src/indexer.cpp
)

add_executable(indexer src/indexer.cpp)
target_link_libraries(indexer common)
