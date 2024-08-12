#include "../include/robot_dirt_sensor.h"

RobotDirtSensor::RobotDirtSensor(){}

int RobotDirtSensor::dirtLevel() const {
    return house->getDirtLevel();
}
