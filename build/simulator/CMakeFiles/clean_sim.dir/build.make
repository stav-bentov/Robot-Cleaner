# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build

# Utility rule file for clean_sim.

# Include any custom commands dependencies for this target.
include simulator/CMakeFiles/clean_sim.dir/compiler_depend.make

# Include the progress variables for this target.
include simulator/CMakeFiles/clean_sim.dir/progress.make

simulator/CMakeFiles/clean_sim:
	/usr/bin/cmake -E rm -rf CMakeFiles CMakeCache.txt Makefile cmake_install.cmake
	/usr/bin/cmake -E rm -rf /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/myrobot

clean_sim: simulator/CMakeFiles/clean_sim
clean_sim: simulator/CMakeFiles/clean_sim.dir/build.make
.PHONY : clean_sim

# Rule to build all files generated by this target.
simulator/CMakeFiles/clean_sim.dir/build: clean_sim
.PHONY : simulator/CMakeFiles/clean_sim.dir/build

simulator/CMakeFiles/clean_sim.dir/clean:
	cd /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build/simulator && $(CMAKE_COMMAND) -P CMakeFiles/clean_sim.dir/cmake_clean.cmake
.PHONY : simulator/CMakeFiles/clean_sim.dir/clean

simulator/CMakeFiles/clean_sim.dir/depend:
	cd /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/simulator /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build/simulator /mnt/c/Stav/Projects/Advanced-topics-in-programming/final-project/Robot-Cleaner/build/simulator/CMakeFiles/clean_sim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : simulator/CMakeFiles/clean_sim.dir/depend

