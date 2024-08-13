#ifndef ROBOT_WALLS_SENSOR_H_
#define ROBOT_WALLS_SENSOR_H_

#include <utility>
#include <vector>
#include <map>
#include <memory>
#include "../../common/WallSensor.h"
#include "robot_sensor.h"
#include "common_enums.h"

class RobotWallsSensor : public WallsSensor, public RobotSensor {
    public:
        RobotWallsSensor();
        RobotWallsSensor(std::shared_ptr<House> _house) 
        : RobotSensor(_house) {}

        RobotWallsSensor(const RobotWallsSensor& other) = default;
        RobotWallsSensor& operator=(const RobotWallsSensor& other) = default;
        ~RobotWallsSensor() override = default;

        bool isWall(Direction d) const override;
};
#endif  // ROBOT_WALLS_SENSOR_H_
