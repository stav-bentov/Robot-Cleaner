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

class MyAlgorithm : public AbstractAlgorithm {
    protected:
        // Sensors
        const WallsSensor* wallsSensor;
        const DirtSensor* dirtSensor;
        const BatteryMeter* batteryMeter;
        
        // Steps information
        std::size_t totalSteps;
        int maxBatterySteps;
        bool firstStep;
    private:
        virtual void updateMapping() = 0;
    public:
        MyAlgorithm(): wallsSensor(nullptr),
                       dirtSensor(nullptr),
                       batteryMeter(nullptr),
                       totalSteps(0),
                       maxBatterySteps(0),
                       firstStep(true){};
        ~MyAlgorithm() override{
            std::cout << "In MyAlgorithm deconstructor" <<std::endl;
        };
        void setMaxSteps(std::size_t maxSteps) override;
        void setWallsSensor(const WallsSensor& sensor) override;
        void setDirtSensor(const DirtSensor& sensor) override;
        void setBatteryMeter(const BatteryMeter& meter) override;
        //Step nextStep() override;
};
#endif  // MY_ALGORITHM_H