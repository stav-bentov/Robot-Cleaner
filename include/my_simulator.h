#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include "my_algorithm.h"
#include "house.h"
#include "output_manager.h"
#include "robot_battery_meter.h"
#include "robot_dirt_sensor.h"
#include "robot_walls_sensor.h"
#include <string>

class MySimulator {
    private:
        // Sensors
        RobotWallsSensor wallsSensor;
        RobotDirtSensor dirtSensor;
        RobotBatteryMeter batteryMeter;

        std::shared_ptr<House> house;

        // Robot details
        std::pair<int, int> dockingStationLocation;
        std::pair<int, int> currentLocation;
        size_t maxSteps;

        // Used algorithm
        AbstractAlgorithm* myAlgo;

        // Handle end run
        OutputManager om;
        
        void logStep(Step s);     
    public:
        MySimulator();
        void readHouseFile(std::string& houseFileName);
        void setAlgorithm(AbstractAlgorithm& algo);
        void run();
};
#endif  // MY_SIMULATOR_H