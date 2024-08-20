#include "../include/my_algorithm.h"
#include <thread>
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
    std::cout <<"in thread " << std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) << "in MyAlgorithm::setBatteryMeter next step maxBatterySteps= : " <<maxBatterySteps << std::endl;    

}

/*
    Update house mapping:
        - Current location dirt
        - walls/ not walls in the nearest neigbors
*/
// TODO: Delete
/*void MyAlgorithm::updateMapping() {
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
}*/

/*Step MyAlgorithm::nextStep() {
    return Step::Finish;
}*/