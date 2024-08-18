#include "../include/Algo_209228600_B.h"
extern "C" 
{
REGISTER_ALGORITHM(Algo_209228600_B);
}

Algo_209228600_B::Algo_209228600_B() {}

/*
    Update house mapping:
        - Current location dirt
        - walls/ not walls in the nearest neigbors
*/
void Algo_209228600_B::updateMapping_() {
	if (dirtSensor) {
		std::cout << "dirtSensor not empty" << std::endl;
	}
	else
	{
		std::cout << "dirtSensor is empty" << std::endl;
	}
    // Update current place amount of dirt
    houseMappingB.setDirt(dirtSensor->dirtLevel());
    // Add current location close vertices
    for (const auto& entry : Common::directionMap) {  
        if (!wallsSensor->isWall(entry.first)) {
            std::cout << "calling addTile: " << std::endl;
            houseMappingB.addTile(entry.first, Type::Floor);
        }
    }
}

Step Algo_209228600_B::nextStep() {
    std::cout << "Algo_209228600_B::nextStep()updateMapping_();" << std::endl;
    updateMapping_();
    std::cout << "Battery state: " << batteryMeter->getBatteryState() << std::endl;
    std::cout << "totalSteps: " << totalSteps << std::endl<< std::endl;
    Step step = houseMappingB.getStepFromMapping(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}