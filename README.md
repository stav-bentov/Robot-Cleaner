Stav Ben-Tov
# Robot-Cleaner

## Project Overview
This project is part of my Advanced Topics in Programming course and simulates a robot cleaner navigating a house. The robot receives a house layout and an algorithm (.so file) that dictates its cleaning strategy. The simulation allows multiple houses and algorithms to be run concurrently using multithreading, managed with semaphores to control resource access.

### Features
- House Layout: The house contains walls, dirt (levels 0-9), and a docking station.
- Algorithm: The cleaning algorithm handles navigation and cleaning, considering battery life and total steps. The robot maintains a memory map of the house, including dirt locations, to optimize its cleaning.
- Battery Management: The robot must return to the docking station to recharge when needed, and it can "die" if it runs out of battery outside the docking station.
- Multithreading: The simulation supports multiple threads to run various house-algorithm combinations simultaneously, using semaphores for thread synchronization.

### Configuration Files
- config/config.json: specifies the maximum time per step (timePerStep). For each thread a timeout is set according to maxSteps*timePerStep, if a thread exceeds this time, it will be stopped mid-process to ensure timely execution.
- config/visualisation_config.json: The simulation can visualize the cleaning process based on the configuration in visualisation_config.json. This file allows you to:
    - Enable or disable the visualization (displaySimulation).
    - Customize the representation of walls, docking stations, and the robot.
    - Set colors for different elements of the simulation.

## Simulation Goals
The robot's primary objective is to clean as much dirt as possible while ensuring it returns to the docking station before the battery runs out. The algorithm balances between cleaning efficiency and battery conservation, using the robot's internal map of the house to improve navigation and cleaning routes.

## Simulation Goals
The simulation is run by dynamically loading house files and algorithm .so files, and creating threads to run multiple simulations in parallel. Semaphores are used to ensure smooth operation when accessing shared resources across threads.

## Compilation and Building

To compile and build the project, follow these steps:

```
cmake -S . -B ./build
cd build
make
```

The algorithm shared object files (.so) will be created in the algorithms-so directory and mytobot exe in main directory (should cd .. from build directory)

## Compilation and Building

To run the robot, use the following commands:
```
./myrobot -algo_path=______ -house_path=_____ -num_threads=_____....

Default values: 
- algo_path and house_path set to current directory
- num_threads is set to its maximum- 10

### Explanation of Algorithms
## Algorithm A
Algorithm A determines the next step based on the destination. The next destination is preferably the closest dirty tile. If no dirty tile is found or if it is not reachable (based on battery steps), the closest unknown tile (if reachable) will be chosen. If no such tile is found, the robot will return to the docking station to charge. 

## Algorithm B
Algorithm B determines the next step based on the destination. Algorithm B randomly selects the next destination from the closest tiles that are either unknown or have known dirt, without giving priority to either type. If no such tile is found, the robot will return to the docking station.

Both algorithms, when they have a destination, build the shortest path to it while maximizing the number of unknown tiles encountered so that the robot can find out dirt of the most tiles.
Also, both algorithms, when the robot returns to the docking station because it has encountered an unreachable destination or has no destination, it will only finish its task if it is at the docking station and the potential destination is too far away, taking into account the maximum steps and battery life. It will not charge unnecessarily.

In conclusion, the difference between the algorithms lies in their decision-making process: Algorithm A prioritizes finding the dirty tiles, preferring them over the closest tiles with unknown dirt, which may result in missing tiles with unknown dirt. In contrast, Algorithm B does not prioritize and gives equal consideration to tiles with unknown dirt and tiles with known dirt, as long as they are the closest options.