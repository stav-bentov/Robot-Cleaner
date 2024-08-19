#ifndef MY_ALGORITHM_D_H
#define MY_ALGORITHM_D_H
#include "../../Common/AbstractAlgorithm.h"
#include "../../algorithm/AlgorithmRegistration.h"
#include <thread>
#include <chrono>
#include <cstddef>
#include <map>
#include <memory>
#include <stack>
#include <iostream>

class Algorithm_D : public AbstractAlgorithm {
private:
    const WallsSensor* wallsSensor;
    const DirtSensor* dirtSensor;
    const BatteryMeter* batteryMeter;
    std::size_t totalSteps;

public:
    Algorithm_D(): wallsSensor(nullptr),
                    dirtSensor(nullptr),
                    batteryMeter(nullptr),
                    totalSteps(0){};
  void setMaxSteps(std::size_t maxSteps) override;
  void setWallsSensor(const WallsSensor &walls_sensor) override;
  void setDirtSensor(const DirtSensor &dirt_sensor) override;
  void setBatteryMeter(const BatteryMeter &battery_meter) override;
  Step nextStep() override;
};
#endif