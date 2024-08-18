#ifndef MY_ALGORITHM_A_H
#define MY_ALGORITHM_A_H

#include <queue>
#include <unordered_set>
#include <memory>
#include <stack>
#include "../../common/AbstractAlgorithm.h"
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/house_mapping_graph.h"
#include "../../algorithm/AlgorithmRegistration.h"

class Algo_209228600_A : public AbstractAlgorithm {
    protected:
        // Sensors
        const WallsSensor* wallsSensor;
        const DirtSensor* dirtSensor;
        const BatteryMeter* batteryMeter;

        // Not actual current location, but relative to docking station
        std::pair<int, int> relativeCurrentLocation;
        HouseMappingGraph houseMapping;
        
        // Steps information
        std::size_t totalSteps;
        int maxBatterySteps;
        bool firstStep;

    public:
        Algo_209228600_A(): wallsSensor(nullptr),
                       dirtSensor(nullptr),
                       batteryMeter(nullptr),
                       relativeCurrentLocation(0, 0), // Initialize currentLocation as needed
                       totalSteps(0),
                       maxBatterySteps(0),
                       firstStep(true){};
        ~Algo_209228600_A() override = default;
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& sensor) override;
        void setDirtSensor(const DirtSensor& sensor) override;
        void setBatteryMeter(const BatteryMeter& meter) override;
        Step nextStep() override;
};
#endif  // MY_ALGORITHM_H