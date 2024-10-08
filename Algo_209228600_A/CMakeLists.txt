cmake_minimum_required(VERSION 3.22)
project(Algo_209228600_A)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Compile options for position-independent code and visibility
add_compile_options(-fPIC -fvisibility=default -fvisibility-inlines-hidden)

# List of source files
set(CURRENT_ALGORITHM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Algo_209228600_A.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HouseMappingGraphA.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/MyAlgorithm.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/HouseMappingGraph.cpp
)

# Create the shared library
add_library(${PROJECT_NAME} SHARED
    ${CURRENT_ALGORITHM_SOURCES}
)

# Set the output directory for the shared library
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/algorithms-so
)

# Set compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)
