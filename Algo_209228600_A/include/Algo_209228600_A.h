#ifndef MY_ALGORITHM_A_H
#define MY_ALGORITHM_A_H

#include <queue>
#include <unordered_set>
#include <memory>
#include <stack>
#include "../../algorithm/AlgorithmRegistration.h"
#include "../../common_algorithm/include/my_algorithm.h"

class Algo_209228600_A: public MyAlgorithm {
    public:
        Algo_209228600_A();
        // TODO: Might need deconstructor
        Step nextStep() override;
};
#endif  // MY_ALGORITHM_A_H