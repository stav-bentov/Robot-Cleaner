#ifndef ROBOT_BATTERY_METER_H_
#define ROBOT_BATTERY_METER_H_

#include "robot_sensor.h"
#include "../../common/BatteryMeter.h"

class RobotBatteryMeter : public BatteryMeter ,public RobotSensor{
    public:
        RobotBatteryMeter(){};
        RobotBatteryMeter(std::shared_ptr<House> _house) 
        : RobotSensor(_house){}

        RobotBatteryMeter(const RobotBatteryMeter& other) = default;
        RobotBatteryMeter& operator=(const RobotBatteryMeter& other) = default;
        ~RobotBatteryMeter() override = default;

        std::size_t getBatteryState() const override;
};
#endif  // ROBOT_BATTERY_METER_H_