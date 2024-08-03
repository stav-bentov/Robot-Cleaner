#include "../simulator/include/robot_sensor.h"

std::shared_ptr<House> RobotSensor::house = nullptr;

RobotSensor::RobotSensor(std::shared_ptr<House> _house) {
    house = _house;
}
