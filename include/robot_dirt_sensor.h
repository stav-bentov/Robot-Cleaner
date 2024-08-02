#ifndef ROBOT_DIRT_SENSOR_H
#define ROBOT_DIRT_SENSOR_H

#include <vector>
#include <memory>
#include "dirt_sensor.h"
#include "house_mapping_graph.h"
#include "robot_sensor.h"

class RobotDirtSensor : public DirtSensor, public RobotSensor {
    public:
        RobotDirtSensor();
        RobotDirtSensor(std::shared_ptr<House> _house) 
        : RobotSensor(_house) {}

        RobotDirtSensor(const RobotDirtSensor& other) = default;
        RobotDirtSensor& operator=(const RobotDirtSensor& other) = default;
        ~RobotDirtSensor() override = default;

        int dirtLevel() const override;
};
#endif  // ROBOT_DIRT_SENSOR_H