cmake_minimum_required(VERSION 3.22)
project(Algo_209228600_A)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Compile options for position-independent code and visibility
add_compile_options(-fPIC -fvisibility=default -fvisibility-inlines-hidden)

include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR})

# List of source files
set(COMMON_ALGORITHM_SOURCES
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/Common.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/ConfigurationManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/House.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/HouseMappingGraph.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/Logger.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/MyAlgorithm.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/OutputManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotBatteryMeter.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotDirtSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotWallsSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/Vertex.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/VisualSimulation.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/InputManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/ErrorManager.cpp
)

set(CURRENT_ALGORITHM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Algo_209228600_A.cpp
)

# Create the shared library
add_library(${PROJECT_NAME} SHARED
    ${COMMON_ALGORITHM_SOURCES}
    ${CURRENT_ALGORITHM_SOURCES}
)

# Set the output directory for the shared library
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/algorithms_so
)

# Set compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)
