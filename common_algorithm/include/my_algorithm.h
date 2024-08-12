#ifndef MY_ALGORITHM_H
#define MY_ALGORITHM_H

#include <queue>
#include <unordered_set>
#include <memory>
#include <stack>
#include "../../common/enums.h"
#include "../../common/AbstractAlgorithm.h"
#include "../../common/WallSensor.h"
#include "../../common/DirtSensor.h"
#include "../../common/BatteryMeter.h"
#include "house.h"
#include "house_mapping_graph.h"

class MyAlgorithm : public AbstractAlgorithm {
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

        void updateMapping();
    public:
        MyAlgorithm(): wallsSensor(nullptr),
                       dirtSensor(nullptr),
                       batteryMeter(nullptr),
                       relativeCurrentLocation(0, 0), // Initialize currentLocation as needed
                       totalSteps(0),
                       maxBatterySteps(0),
                       firstStep(true){};
        ~MyAlgorithm() override = default;
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& sensor) override;
        void setDirtSensor(const DirtSensor& sensor) override;
        void setBatteryMeter(const BatteryMeter& meter) override;
        Step nextStep() override;
};
#endif  // MY_ALGORITHM_H