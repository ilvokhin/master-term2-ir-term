cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED serialization filesystem system thread)

SET(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++11 -pedantic")


include_directories(
  include
  OleanderStemmingLibrary
)

add_library(common src/common.cpp)

add_library(indexer src/indexer.cpp)

add_library(searcher src/searcher.cpp)
target_link_libraries(searcher indexer common ${Boost_LIBRARIES})

add_library(merger src/merger.cpp)
target_link_libraries(merger indexer common searcher ${Boost_LIBRARIES})

set(SOURCES 
  src/run_indexer.cpp
  src/run_searcher.cpp
  src/run_merger.cpp
  src/tcp_worker.cpp
)

add_executable(run_indexer src/run_indexer.cpp)
target_link_libraries(run_indexer indexer common ${Boost_LIBRARIES})

add_executable(run_searcher src/run_searcher.cpp)
target_link_libraries(run_searcher indexer common searcher ${Boost_LIBRARIES})

add_executable(run_merger src/run_merger.cpp)
target_link_libraries(run_merger indexer common searcher merger ${Boost_LIBRARIES})

add_executable(tcp_worker src/tcp_worker.cpp)
target_link_libraries(tcp_worker indexer searcher common ${Boost_LIBRARIES})
