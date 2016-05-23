cmake_minimum_required(VERSION 2.8)

find_package(Boost REQUIRED serialization filesystem system thread)

SET(CMAKE_CXX_FLAGS "-Wall -O3 -std=c++11 -pedantic -g")


include_directories(
  include
  OleanderStemmingLibrary
)

add_library(common src/common.cpp)

add_library(indexer src/indexer.cpp)

add_library(idf_table src/idf_table.cpp)
target_link_libraries(idf_table indexer common ${Boost_LIBRARIES})

add_library(searcher src/searcher.cpp src/ranker.cpp)
target_link_libraries(searcher idf_table indexer common ${Boost_LIBRARIES})

add_library(merger src/merger.cpp)
target_link_libraries(merger indexer common searcher ${Boost_LIBRARIES})


set(SOURCES 
  src/run_indexer.cpp
  src/run_searcher.cpp
  src/run_merger.cpp
  src/tcp_worker.cpp
  src/run_calc_idf.cpp
  src/get_idf.cpp
)

add_executable(run_indexer src/run_indexer.cpp)
target_link_libraries(run_indexer indexer common ${Boost_LIBRARIES})

add_executable(run_searcher src/run_searcher.cpp)
target_link_libraries(run_searcher indexer common searcher ${Boost_LIBRARIES})

add_executable(run_merger src/run_merger.cpp)
target_link_libraries(run_merger indexer common searcher merger ${Boost_LIBRARIES})

add_executable(tcp_worker src/tcp_worker.cpp)
target_link_libraries(tcp_worker idf_table indexer searcher common ${Boost_LIBRARIES})

add_executable(run_calc_idf src/run_calc_idf.cpp)
target_link_libraries(run_calc_idf idf_table indexer common ${Boost_LIBRARIES})

add_executable(get_idf src/get_idf.cpp)
target_link_libraries(get_idf idf_table common ${Boost_LIBRARIES})

