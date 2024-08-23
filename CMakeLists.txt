cmake_minimum_required(VERSION 3.22)
project(RobotCleaner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

# Add subdirectories
add_subdirectory(Algo_209228600_A)
add_subdirectory(Algo_209228600_B)
add_subdirectory(simulator)

# List of common libraries
set(common_libs 
    Algo_209228600_A
    Algo_209228600_B
)