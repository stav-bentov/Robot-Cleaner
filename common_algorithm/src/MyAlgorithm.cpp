#include "../include/my_algorithm.h"

void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
    std::cout << "MyAlgorithm::setMaxSteps" << std::endl;

    totalSteps = maxSteps;
} 

void MyAlgorithm::setWallsSensor(const WallsSensor& sensor) {
    std::cout << "MyAlgorithm::setWallsSensor" << std::endl;
    wallsSensor = &sensor;
}

void MyAlgorithm::setDirtSensor(const DirtSensor& sensor) {
    std::cout << "MyAlgorithm::setDirtSensor" << std::endl;
    dirtSensor = &sensor; 
}

void MyAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
    std::cout << "MyAlgorithm::setBatteryMeter" << std::endl;
    batteryMeter = &meter;
    maxBatterySteps = batteryMeter->getBatteryState();
}

/*
    Update house mapping:
        - Current location dirt
        - walls/ not walls in the nearest neigbors
*/
void MyAlgorithm::updateMapping() {
    // Update current place amount of dirt
    houseMapping.setDirt(dirtSensor->dirtLevel());

    // Add current location close vertices
    for (const auto& entry : Common::directionMap) {  
        if (wallsSensor->isWall(entry.first)) {
            houseMapping.addVertex(entry.first, Type::Wall);
        }
        else {
            houseMapping.addVertex(entry.first, Type::Floor);
        }
    }
}

/*
    The Flow:
        - Update BFS with current vertex and it's sorrundings
        - Run Bfs
        - If need to get back to the station- get back
        - Else- if current location is dirty - clean
            - Else- Check if the distance from nearest dirt location + its distance from docking station is less then battery steps- go in this direction
                - Else- go back to docking station
*/
Step MyAlgorithm::nextStep() {
    
    std::cout <<" Algo_209228600_A::nextStep()" << std::endl;
    // Edge case (maximum steps for battery is 1 so there is no place to go)
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }

    // No more steps left to do
    if (totalSteps == 0) {
        // Should be in docking station!
        return Step::Finish;
    }

    // The algorithm should strive to return “Finished” when on dock and the remaining amount of steps (remaining from the given MaxSteps), 
    // would not allow cleaning any additional dirt and getting back to the docking on time.
    if (houseMapping.shouldFinish()) {
        //Logger::getInstance().getLogger()->info("Should finish");
        return Step::Finish;
    }

    // Make the step in algorithm
    //std::cout << "updateMapping()" << std::endl;
    std::cout <<" updateMapping();" << std::endl;
    updateMapping();
    std::cout <<" Step step = houseMapping.runBfs(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);" << std::endl;
    //std::cout << "Step step = houseMapping.runBfs(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);" << std::endl;
    Step step = houseMapping.runBfs(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}