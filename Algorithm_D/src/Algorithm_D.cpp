#include "../include/Algorithm_D.h"

REGISTER_ALGORITHM(Algorithm_D);

void Algorithm_D::setMaxSteps(std::size_t maxSteps) {
    std::cout << "MyAlgorithm::setMaxSteps" << std::endl;
    totalSteps = maxSteps;
} 

void Algorithm_D::setWallsSensor(const WallsSensor& sensor) {
    std::cout << "MyAlgorithm::setWallsSensor" << std::endl;
    wallsSensor = &sensor;
}

void Algorithm_D::setDirtSensor(const DirtSensor& sensor) {
    std::cout << "MyAlgorithm::setDirtSensor" << std::endl;
    dirtSensor = &sensor; 
}

void Algorithm_D::setBatteryMeter(const BatteryMeter& meter) {
    std::cout << "MyAlgorithm::setBatteryMeter" << std::endl;
    batteryMeter = &meter;
}


Step Algorithm_D::nextStep() {
    std::cout << " " << std::endl;
    std::cout << "Algorithm_D::nextStep()" << std::endl;
    std::cout << " " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return Step::Finish;
}