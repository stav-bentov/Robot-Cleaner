#include "../simulator/include/my_algorithm.h"

void MyAlgorithm::setMaxSteps(std::size_t maxSteps) {
    totalSteps = maxSteps;
} 

void MyAlgorithm::setWallsSensor(const WallsSensor& sensor) {
    wallsSensor = &sensor;
}

void MyAlgorithm::setDirtSensor(const DirtSensor& sensor) {
    dirtSensor = &sensor; 
}

void MyAlgorithm::setBatteryMeter(const BatteryMeter& meter) {
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
    houseMapping.setDirt({relativeCurrentLocation.first, relativeCurrentLocation.second}, dirtSensor->dirtLevel());

    // Add current location close vertices
    for (const auto& entry : House::directionMap) {  
        if (wallsSensor->isWall(entry.first)) {
            houseMapping.addVertex(entry.first, Type::Wall);
        }
        else {
            houseMapping.addVertex(entry.first, Type::Floor);
        }
        houseMapping.connect(entry.first);
    }
    Logger::getInstance().getLogger()->info("Done update mapping in algo");
}