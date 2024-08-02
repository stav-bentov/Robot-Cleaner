#include "Algo_209228600_A.h"

REGISTER_ALGORITHM(Algo_209228600_A);

/*
    The Flow:
        - Update BFS with current vertex and it's sorrundings
        - Run Bfs
        - If need to get back to the station- get back
        - Else- if current location is dirty - clean
            - Else- Check if the distance from nearest dirt location + its distance from docking station is less then battery steps- go in this direction
                - Else- go back to docking station
*/
Step Algo_209228600_A::nextStep() {
    // Edge case
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }

    if (totalSteps == 0) {
        // Should be in docking station!
        return Step::Finish;
    }

    // The algorithm should strive to return “Finished” when on dock and the remaining amount of steps (remaining from the given MaxSteps), 
    // would not allow cleaning any additional dirt and getting back to the docking on time.
    if (houseMapping.shouldFinish()) {
        Logger::getInstance().getLogger()->info("Should finish");
        return Step::Finish;
    }

    // Make the step in algorithm
    updateMapping();
    Step step = houseMapping.runBfs(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}