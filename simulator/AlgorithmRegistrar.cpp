#include "../common/AlgorithmRegistrar.h"

//-----------------------------------
// simulator/AlgorithmRegistrar.cpp
//-----------------------------------
AlgorithmRegistrar AlgorithmRegistrar::registrar;

AlgorithmRegistrar& AlgorithmRegistrar::getAlgorithmRegistrar() { return registrar; }