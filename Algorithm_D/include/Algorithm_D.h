#ifndef MY_ALGORITHM_D_H
#define MY_ALGORITHM_D_H
#include "../../Common/AbstractAlgorithm.h"
#include "../../algorithm/AlgorithmRegistration.h"
#include "../../common_algorithm/include/my_algorithm.h"
#include "house_mapping_graph.h"
#include <thread>
#include <chrono>
#include <cstddef>
#include <map>
#include <memory>
#include <unordered_set>
#include <stack>
#include <iostream>

class Algorithm_D : public MyAlgorithm {
    public:
        Step nextStep() override;
        
    private:
        void updateMapping() override;
        HouseMappingGraph houseMapping;
};
#endif