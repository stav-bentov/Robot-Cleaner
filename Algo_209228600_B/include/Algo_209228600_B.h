#ifndef MY_ALGORITHM_B_H
#define MY_ALGORITHM_B_H

#include <queue>
#include <unordered_set>
#include <memory>
#include <stack>
#include "../../algorithm/AlgorithmRegistration.h"
#include "../../common/WallSensor.h"
#include "../../common/DirtSensor.h"
#include "../../common/BatteryMeter.h"
#include "../../common_algorithm/include/my_algorithm.h"
#include "house_mapping_graph_B.h"

class Algo_209228600_B: public MyAlgorithm {
    private:
        HouseMappingGraphB houseMappingB;
        void updateMapping_();
    public:
        Algo_209228600_B();
        Step nextStep() override;
};
#endif  // MY_ALGORITHM_B_H