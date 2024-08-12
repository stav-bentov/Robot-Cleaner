cmake_minimum_required(VERSION 3.22)
project(RobotCleaner)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

add_subdirectory(Algo_209228600_A)
add_subdirectory(simulator)

set(common_libs 
  Algorithm_209229600_A
  )
