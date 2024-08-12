#include "../include/my_algorithm.h"

void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
	std::cout << "in setMaxSteps" << std::endl;
    totalSteps = maxSteps;
} 

void MyAlgorithm::setWallsSensor(const WallsSensor& sensor) {
	std::cout << "in setWallsSensor" << std::endl;
    wallsSensor = &sensor;
}

void MyAlgorithm::setDirtSensor(const DirtSensor& sensor) {
	std::cout << "in setDirtSensor" << std::endl;
    dirtSensor = &sensor; 
}

void MyAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
	std::cout << "in setBatteryMeter" << std::endl;
    batteryMeter = &meter;
	std::cout << "batteryMeter = &meter" << std::endl;
    maxBatterySteps = batteryMeter->getBatteryState();
	std::cout << "maxBatterySteps = batteryMeter->getBatteryState()" << std::endl;
	std::cout << "end setBatteryMeter" << std::endl;
}

/*
    Update house mapping:
        - Current location dirt
        - walls/ not walls in the nearest neigbors
*/
void MyAlgorithm::updateMapping() {
    Logger::getInstance().log("MyAlgorithm::updateMapping - Setting dirt at current location.\n", 3);

    // Update current place amount of dirt
    houseMapping.setDirt(dirtSensor->dirtLevel());
    Logger::getInstance().log("MyAlgorithm::updateMapping - Dirt set at current location. Adding neighbors.\n", 3);

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