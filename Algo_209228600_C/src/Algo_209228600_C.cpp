#include "../include/Algo_209228600_C.h"

extern "C" 
{
REGISTER_ALGORITHM(Algo_209228600_C);
}

Algo_209228600_C::Algo_209228600_C() : 
                            totalSteps(0),
                            wallsSensor(nullptr),
                            dirtSensor(nullptr),
                            batteryMeter(nullptr),
                            firstStep(true),
                            relativeCurrentLocation(0, 0), // Initialize currentLocation as needed
                            houseMapping(relativeCurrentLocation),
                            maxBatterySteps(0){}

void Algo_209228600_C::setMaxSteps(std::size_t maxSteps) {
    totalSteps = maxSteps;
} 

void Algo_209228600_C::setWallsSensor(const WallsSensor& sensor) {
    wallsSensor = &sensor;
}

void Algo_209228600_C::setDirtSensor(const DirtSensor& sensor) {
    dirtSensor = &sensor; 
}

void Algo_209228600_C::setBatteryMeter(const BatteryMeter& meter) {
    batteryMeter = &meter;
    maxBatterySteps = batteryMeter->getBatteryState();
}

/*
    Update house mapping:
        - Current location dirt
        - walls/ not walls in the nearest neigbors
*/
void Algo_209228600_C::updateMapping() {
    // If this is the first step- add the first vertix - (0,0) docking station
    if (firstStep) {
        houseMapping.addVertex({relativeCurrentLocation.first, relativeCurrentLocation.second}, Type::DockingStation);
        firstStep = false;
    }

    // If it's not first step then current row and col is already in the mapping, 
    // Because we are on this spot we know its not a wall.
    // We can know the dirt only now
    houseMapping.setDirt({relativeCurrentLocation.first, relativeCurrentLocation.second}, dirtSensor->dirtLevel());

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
Step Algo_209228600_C::nextStep() {
    // Edge case
    if (maxBatterySteps == 1) {
        return Step::Finish;
    }

    if (totalSteps == 0) {
        // Should be in docking station!
        return Step::Finish;
    }

    // The algorithm should strive to return “Finished” when on dock and the remaining amount of steps (remaining from the given MaxSteps), 
    // would not allow cleaning any additional dirt and getting back to the docking on time.
    if (!firstStep && houseMapping.shouldFinish()) {
        return Step::Finish;
    }

    // Make the step in algorithm
    updateMapping();
    Step step = houseMapping.runBfs(batteryMeter->getBatteryState(), maxBatterySteps, totalSteps);
    totalSteps--;
    return step;
}