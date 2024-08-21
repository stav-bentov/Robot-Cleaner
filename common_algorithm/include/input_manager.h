#ifndef INPUT_MANAGER_H_
#define INPUT_MANAGER_H_

#include <memory>
#include <cstddef>
#include <algorithm>  
#include <stdexcept>
#include <type_traits>
#include "common_enums.h"

class InputManager {
    public:
        InputManager(const std::string& housePath);
        void getParameters(std::string& _houseName, std::size_t& _maxSteps, std::size_t& _maxBattery, int& _amountOfDirt,
                int& _rows, int& _cols, std::pair<int, int>& _dockingStationLocation, std::vector<std::vector<int>>& _houseSurface) const;
    private:
        // Input parameters
        std::string houseName;
        std::size_t maxSteps;
        std::size_t maxBattery;
        int amountOfDirt;
        int rows;
        int cols;
        std::pair<int, int> dockingStationLocation; 
        std::vector<std::vector<int>> houseSurface;

        void processParametersFromFile(std::ifstream& file);
        void processHouseMappingFromFile(std::ifstream& file);
        template<typename T>
        void processVariable(std::ifstream& file, T& var, const std::string& errorMessage);
    
};
#endif  // INPUT_MANAGER_H_