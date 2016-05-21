cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED serialization)

SET(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++11 -pedantic")


include_directories(
  include
  OleanderStemmingLibrary
)

add_library(common src/common.cpp)
add_library(indexer src/indexer.cpp)
add_library(searcher src/searcher.cpp)

set(SOURCES 
  src/run_indexer.cpp
  src/run_searcher.cpp
)

add_executable(run_indexer src/run_indexer.cpp)
target_link_libraries(run_indexer indexer common ${Boost_LIBRARIES})

add_executable(run_searcher src/run_searcher.cpp)
target_link_libraries(run_searcher indexer common searcher ${Boost_LIBRARIES})
