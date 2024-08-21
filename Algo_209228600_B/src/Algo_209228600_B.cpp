#include "../include/Algo_209228600_B.h"

REGISTER_ALGORITHM(Algo_209228600_B);

void Algo_209228600_B::updateMapping() {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    std::cout << thread <<"B::updateMapping()" << std::endl;
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

Step Algo_209228600_B::nextStep() {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    Logger::getInstance().log(thread + " maxBatterySteps = "+std::to_string(maxBatterySteps) +".\n", LogLevels::FILE);

    std::cout << "in algorithm B next step maxBatterySteps= : " <<maxBatterySteps << "in thread " << std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) << std::endl;    
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }
    
    std::cout << thread <<"Algorithm_B::nextStep()" << std::endl;
    updateMapping();
    Step step = houseMapping.getStepFromMapping(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}