
#include "../include/Algo_209228600_A.h"
REGISTER_ALGORITHM(Algo_209228600_A);

void Algo_209228600_A::setMaxSteps(std::size_t maxSteps) {
    totalSteps = maxSteps;
} 

void Algo_209228600_A::setWallsSensor(const WallsSensor& sensor) {
    wallsSensor = &sensor;
}

void Algo_209228600_A::setDirtSensor(const DirtSensor& sensor) {
    dirtSensor = &sensor; 
}

void Algo_209228600_A::setBatteryMeter(const BatteryMeter& meter) {
    batteryMeter = &meter;
    maxBatterySteps = batteryMeter->getBatteryState();
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
Step Algo_209228600_A::nextStep() {
    return Step::Finish;
}