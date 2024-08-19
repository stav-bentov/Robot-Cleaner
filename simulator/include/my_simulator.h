#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include "../../common_algorithm/include/my_algorithm.h"
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/output_manager.h"
#include "../../common_algorithm/include/robot_battery_meter.h"
#include "../../common_algorithm/include/robot_dirt_sensor.h"
#include "../../common_algorithm/include/robot_walls_sensor.h"
#include <string>
#include <atomic>

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
        int score;
        std::atomic<bool>& stopFlag; // Reference to the stop flag
        void setSensors();
        void setHouse(std::string houseFileName);
        int calculateScore(std::size_t numSteps, std::string status, int amountOfDirtLeft, bool inDocking, std::size_t maxSteps);

    public:
        MySimulator(std::atomic<bool>& stopFlag_);
        void prepareSimulationEnvironment(std::string houseFilePath, std::string algoName) ;
        void setAlgorithm(AbstractAlgorithm& algo);
        void run();
        void setOutput();
        int getScore() const;

};
#endif  // MY_SIMULATOR_H