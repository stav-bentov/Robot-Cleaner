Stav Ben-Tov
# Robot-Cleaner

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
./myrobot 
```
or
```
./myrobot -algo_path=______ -house_path=_____ ....
```
### Explanation of Algorithms
## Algorithm A
Algorithm A determines the next step based on the destination. The next destination is preferably the closest dirty tile. If no dirty tile is found or if it is not reachable (based on battery steps), the closest unknown tile (if reachable) will be chosen. If no such tile is found, the robot will return to the docking station to charge. 

## Algorithm B
Algorithm B determines the next step based on the destination. Algorithm B randomly selects the next destination from the closest tiles that are either unknown or have known dirt, without giving priority to either type. If no such tile is found, the robot will return to the docking station.

Both algorithms, when they have a destination, build the shortest path to it while maximizing the number of unknown tiles encountered so that the robot can find out dirt of the most tiles.
Also, both algorithms, when the robot returns to the docking station because it has encountered an unreachable destination or has no destination, it will only finish its task if it is at the docking station and the potential destination is too far away, taking into account the maximum steps and battery life. It will not charge unnecessarily.

In conclusion, the difference between the algorithms lies in their decision-making process: Algorithm A prioritizes finding the dirty tiles, preferring them over the closest tiles with unknown dirt, which may result in missing tiles with unknown dirt. In contrast, Algorithm B does not prioritize and gives equal consideration to tiles with unknown dirt and tiles with known dirt, as long as they are the closest options.