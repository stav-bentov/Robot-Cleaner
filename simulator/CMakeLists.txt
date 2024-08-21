cmake_minimum_required(VERSION 3.22)
project(simulator)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_compile_options(-fPIC)

# Set linker flags for dynamic linking
set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR})

# List of source files
set(COMMON_SOURCES
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/Common.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/ConfigurationManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/Logger.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/OutputManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotBatteryMeter.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotDirtSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/RobotWallsSensor.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/VisualSimulation.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/InputManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/ErrorManager.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/House.cpp
    ${CMAKE_SOURCE_DIR}/common_algorithm/src/VisualConfigManager.cpp
)

# Add the executable target
add_executable(myrobot
    ${COMMON_SOURCES}
    ${PROJECT_SOURCE_DIR}/main.cpp 
    ${PROJECT_SOURCE_DIR}/AlgorithmRegistrar.cpp 
    ${PROJECT_SOURCE_DIR}/MySimulator.cpp
    ${PROJECT_SOURCE_DIR}/MainManager.cpp
    ${PROJECT_SOURCE_DIR}/CsvManager.cpp
    ${PROJECT_SOURCE_DIR}/SimConfigManager.cpp
    ${PROJECT_SOURCE_DIR}/Task.cpp
)

# Link the dl library
target_link_libraries(myrobot PRIVATE dl)

# Set the output directory for the executable
set_target_properties(myrobot PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Link additional libraries to the executable
target_link_libraries(myrobot PRIVATE
    ${common_libs} # Ensure common_libs is defined and includes required libraries
    dl
    pthread
)

# Set compiler options
target_compile_options(myrobot PRIVATE
    -Wall
    -Wextra
    -Werror
    -pedantic
)

# Add -rdynamic to linker options
target_link_options(myrobot PRIVATE -rdynamic -ldl)