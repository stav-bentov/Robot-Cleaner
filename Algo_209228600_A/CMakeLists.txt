cmake_minimum_required(VERSION 3.22)
project(Algo_209228600_A LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#TODO: Check if to change
add_compile_options(-fPIC -fvisibility=hidden -fvisibility-inlines-hidden)
add_library(Algo_209228600_A SHARED Algo_209228600_A.cpp)

target_include_directories(Algo_209228600_A PUBLIC include)

set_target_properties(Algo_209228600_A PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)