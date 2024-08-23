#ifndef MY_SIMULATOR_H
#define MY_SIMULATOR_H

#include "../../common_algorithm/include/my_algorithm.h"
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/output_manager.h"
#include "../../common_algorithm/include/robot_battery_meter.h"
#include "../../common_algorithm/include/robot_dirt_sensor.h"
#include "../../common_algorithm/include/robot_walls_sensor.h"
#include "../include/sim_config_manager.h"
#include <string>

class MySimulator {
    private:
        // Used algorithm
        std::unique_ptr<AbstractAlgorithm> myAlgo;

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
	    std::vector<Step> steps;
	    std::string status;
        std::string housePath;
        std::string algoName;

        // Cahnged throgh the running
        size_t numberOfStepsMade;
        std::chrono::milliseconds timeoutDuration;
        int score;

        void setSensors();
        void setHouse(std::shared_ptr<House> housePtr);
        void getTimeout();

    public:
        MySimulator();
        MySimulator(MySimulator&& other) noexcept = default;
        MySimulator& operator=(MySimulator&& other) noexcept = default;
        
        ~MySimulator() {
          //  std::cout << "In deconstructor" <<std::endl;
        };

        void prepareSimulationEnvironment(std::shared_ptr<House> housePtr, std::string& housePath_, std::string& algoName_);
        void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algo);
        void run();
        void setOutput();
        int getScore();
        void calculateScore();
        int getnumberOfStepsMade();
};
#endif  // MY_SIMULATOR_H