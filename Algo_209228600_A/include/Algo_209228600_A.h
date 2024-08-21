#ifndef MY_ALGORITHM_A_H
#define MY_ALGORITHM_A_H
#include "../../Common/AbstractAlgorithm.h"
#include "../../algorithm/AlgorithmRegistration.h"
#include "../../common_algorithm/include/my_algorithm.h"
#include "house_mapping_graph_A.h"
#include <thread>
#include <chrono>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <stack>
#include <iostream>

class Algo_209228600_A : public MyAlgorithm {
    public:
        Step nextStep() override;
        
    private:
        void updateMapping() override;
        HouseMappingGraphA houseMapping;
};
#endif