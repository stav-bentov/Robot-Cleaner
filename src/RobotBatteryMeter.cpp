#include "../simulator/include/robot_battery_meter.h"

std::size_t RobotBatteryMeter::getBatteryState() const {
    // Will round down
    return static_cast<size_t>(house->getCurentBatterySteps());
}
