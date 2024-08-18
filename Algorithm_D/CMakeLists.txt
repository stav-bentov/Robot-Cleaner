cmake_minimum_required(VERSION 3.22)
project(Algorithm_D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Compile options for position-independent code and visibility
add_compile_options(-fPIC -fvisibility=default -fvisibility-inlines-hidden)

# List of source files
set(CURRENT_ALGORITHM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Algorithm_D.cpp
)

# Create the shared library
add_library(${PROJECT_NAME} SHARED
    ${CURRENT_ALGORITHM_SOURCES}
)

# Set the output directory for the shared library
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/algorithms_so_output
)

# Set compiler options
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)


#[[ cmake_minimum_required(VERSION 3.22)

project(Algorithm_D)

set(CMAKE_CXX_STANDARD 20)

SET(CMAKE_SHARED_LINKER_FLAGS "-fPIC -shared -fvisibility=hidden -fvisibility-inlines-hidden")

# DO NOT TOUCH
set(COMMON_INCLUDES 
    ${PROJECT_SOURCE_DIR}/common/enums.h
    ${PROJECT_SOURCE_DIR}/common/AbstractAlgorithm.h
    ${PROJECT_SOURCE_DIR}/common/BatteryMeter.h
    ${PROJECT_SOURCE_DIR}/common/DirtSensor.h
    ${PROJECT_SOURCE_DIR}/common/WallsSensor.h
    ${PROJECT_SOURCE_DIR}/common/AlgorithmRegistrar.h)


set(ALGO_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/Algorithm/include/Algorithm_D.h
    )

add_library(${PROJECT_NAME} SHARED 
    ${PROJECT_SOURCE_DIR}/src/Algorithm_D.cpp
    )

target_include_directories(${PROJECT_NAME} PUBLIC
    ${COMMON_INCLUDES}
    ${ALGO_INCLUDES}
) ]]