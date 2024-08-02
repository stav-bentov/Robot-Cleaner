#include "../common/AlgorithmRegistrar.h"

//------------------------------------
// algorithm/AlgorithmRegistration.h
//------------------------------------
struct AlgorithmRegistration {
    AlgorithmRegistration(const std::string& name, AlgorithmFactory algorithmFactory) {
        AlgorithmRegistrar::getAlgorithmRegistrar()
.registerAlgorithm(name, std::move(algorithmFactory));
    }
};

#define REGISTER_ALGORITHM(ALGO) AlgorithmRegistration \
   _##ALGO(#ALGO, []{return std::make_unique<ALGO>();})
