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
}
