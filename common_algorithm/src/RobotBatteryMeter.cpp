#include "../include/robot_battery_meter.h"

std::size_t RobotBatteryMeter::getBatteryState() const {
	std::cout << "before getBatteryState" << std::endl;
    if (house) {
	std::cout << "house not empty" << std::endl;
    }
    else{
	std::cout << "house empty" << std::endl;
    }
    // Will round down
    return static_cast<size_t>(house->getCurentBatterySteps());
}
