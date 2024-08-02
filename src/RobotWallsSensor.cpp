#include "../include/robot_walls_sensor.h"

RobotWallsSensor::RobotWallsSensor(){}

bool RobotWallsSensor::isWall(Direction d) const {
    return house->isWall(d);
}