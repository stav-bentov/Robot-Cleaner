#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include "../../common_algorithm/include/my_algorithm.h"
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/output_manager.h"
#include "../../common_algorithm/include/robot_battery_meter.h"
#include "../../common_algorithm/include/robot_dirt_sensor.h"
#include "../../common_algorithm/include/robot_walls_sensor.h"
#include <string>

class MySimulator {
    private:
        // Used algorithm
        AbstractAlgorithm* myAlgo;

        // Sensors
        RobotWallsSensor wallsSensor;
        RobotDirtSensor dirtSensor;
        RobotBatteryMeter batteryMeter;

        std::shared_ptr<House> house;

        // Robot details
        std::pair<int, int> dockingStationLocation;
        std::pair<int, int> currentLocation;
        size_t maxSteps;

        // Handle end run
        OutputManager om;  
	    std::vector<Step> steps;
	    std::string status;

        // Cahnged throgh the running
        size_t numberOfStepsMade;
        
        void setSensors();
        void setHouse(std::string houseFileName);
    public:
        MySimulator();
        void prepareSimulationEnvironment(std::string houseFilePath, std::string algoName) ;
        void setAlgorithm(AbstractAlgorithm& algo);
        void run();
        void setOutput();
};
#endif  // MY_SIMULATOR_H