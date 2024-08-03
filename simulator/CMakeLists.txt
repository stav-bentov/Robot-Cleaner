cmake_minimum_required(VERSION 3.22)
project(simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_EXE_LINKER_FLAGS "-rdynamic -ldl")
add_executable(simulator main.cpp AlgorithmRegistrar.cpp MySimulator.cpp)
target_include_directories(simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../algo1_lib)
target_link_libraries(simulator PRIVATE 
    Algo_209228600_A 
)