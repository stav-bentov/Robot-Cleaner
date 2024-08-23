#include "../include/Algo_209228600_A.h"

REGISTER_ALGORITHM(Algo_209228600_A);

void Algo_209228600_A::updateMapping() {
    bool isVisited = houseMapping.isVisitedInCurrentLocation();
    // Update current place amount of dirt
    houseMapping.setDirt(dirtSensor->dirtLevel());
    
    // Add information on nearest tiles only if not visited yet (because its a waste to do this twice)
    if (!isVisited) {
        // Add current location close vertices
        for (const auto& entry : Common::directionMap) {  
            if (!wallsSensor->isWall(entry.first)) {
                houseMapping.addTile(entry.first, Type::Floor);
            }
        }
    }
}

Step Algo_209228600_A::nextStep() {
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }
    
    updateMapping();
    Step step = houseMapping.getStepFromMapping(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}