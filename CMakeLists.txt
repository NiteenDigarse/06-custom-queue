cmake_minimum_required(VERSION 3.20)

project(CustomQueue
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(custom_queue_tests
    tests/test_custom_queue.cpp
)

target_include_directories(custom_queue_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(custom_queue_benchmark
    benchmarks/benchmark_custom_queue.cpp
)

target_include_directories(custom_queue_benchmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)