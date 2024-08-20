#ifndef ROBOT_SENSOR_H_
#define ROBOT_SENSOR_H_

#include <utility>
#include <vector>
#include <memory>
#include <iostream>
#include "house.h"

class RobotSensor {
    protected:
        // Static members that will be mutual to all sensors
        std::shared_ptr<House> house; // shared because mySimulator will point at it too

    public:
        RobotSensor() : house(nullptr){}
        RobotSensor(std::shared_ptr<House> _house);
        virtual ~RobotSensor() {}
};

#endif  // ROBOT_SENSOR_H_