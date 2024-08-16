#ifndef MY_ALGORITHM_C_H
#define MY_ALGORITHM_C_H

#include <queue>
#include <unordered_set>
#include <memory>
#include <stack>
#include "../../algorithm/AlgorithmRegistration.h"
#include "../../common/AbstractAlgorithm.h"
#include "house_mapping_graph_c.h"

class Algo_209228600_C: public AbstractAlgorithm{
    private:
        std::size_t totalSteps;
        const WallsSensor* wallsSensor;
        const DirtSensor* dirtSensor;
        const BatteryMeter* batteryMeter;
        bool firstStep;
        // Not actual current location, but relative to docking station
        std::pair<int, int> relativeCurrentLocation;
        HouseMappingGraphC houseMapping;
        int maxBatterySteps;
        void updateMapping();
    public:
        Algo_209228600_C();
        ~Algo_209228600_C() override = default;
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& sensor) override;
        void setDirtSensor(const DirtSensor& sensor) override;
        void setBatteryMeter(const BatteryMeter& meter) override;
        Step nextStep() override;
};
#endif  // MY_ALGORITHM_C_H