#include "../include/Algorithm_D.h"
#include <thread>
REGISTER_ALGORITHM(Algorithm_D);

void Algorithm_D::updateMapping() {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    std::cout << thread <<"Algorithm_D::updateMapping()" << std::endl;
    bool isVisited = houseMapping.isVisitedInCurrentLocation();
    // Update current place amount of dirt
    houseMapping.setDirt(dirtSensor->dirtLevel());
    
    // Add information on nearest tiles only if not visited yet (because its a waste to do this twice)
    if (!isVisited) {
        // Add current location close vertices
        for (const auto& entry : Common::directionMap) {  
            if (!wallsSensor->isWall(entry.first)) {
                std::cout << thread <<"calling addTile: " << std::endl;
                houseMapping.addTile(entry.first, Type::Floor);
            }
        }
    }
}

Step Algorithm_D::nextStep() {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    Logger::getInstance().log(thread + " maxBatterySteps = "+std::to_string(maxBatterySteps) +".\n", LogLevels::FILE);

    //std::cout << "in algorithm D next step maxBatterySteps= : " <<maxBatterySteps << "in thread " << std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) << std::endl;    
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }
    
    std::cout << thread <<"Algorithm_D::nextStep()" << std::endl;
    updateMapping();
    Step step = houseMapping.getStepFromMapping(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}