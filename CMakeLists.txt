cmake_minimum_required(VERSION 3.10)
project(TimerBenchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(benchmark REQUIRED)

# Vectors Benchmarks
add_executable(BenchmarkVectorSwap benchmark_vector_swap.cpp)
target_include_directories(BenchmarkVectorSwap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BenchmarkVectorSwap benchmark::benchmark)

add_executable(BenchmarkVectorEraseIf benchmark_vector_erase_if.cpp)
target_include_directories(BenchmarkVectorEraseIf PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BenchmarkVectorEraseIf benchmark::benchmark)

# Map Benchmarks
add_executable(BenchmarkMap benchmark_map.cpp)
target_include_directories(BenchmarkMap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BenchmarkMap benchmark::benchmark)

# Unordered Map Benchmarks
add_executable(BenchmarkUnorderedMap benchmark_unordered_map.cpp)
target_include_directories(BenchmarkUnorderedMap PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(BenchmarkUnorderedMap benchmark::benchmark)