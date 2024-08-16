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
    return Step::Finish;
}