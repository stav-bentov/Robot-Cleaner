#ifndef HOUSE_H
#define HOUSE_H

#include <memory>
#include <cstddef>
#include <algorithm>  
#include <stdexcept>
#include <type_traits>
#include "../../common/WallSensor.h"
#include "common_enums.h"
#include "input_manager.h"
#include <filesystem>
#include <fstream>

class House {
    private:
        std::string houseName;
        std::size_t maxSteps;
        std::size_t maxBattery;
        int amountOfDirt;
        int rows;
        int cols;
        std::pair<int, int> dockingStationLocation; 
        std::string errorFileName;

        // Changed throgh the running
        double currentBatterySteps;
        std::pair<int, int> currentLocation; 
        std::vector<std::vector<int>> houseSurface;
        
        void charge();
        void discharge();
        void clean();
        bool isWall(std::pair<int, int> location) const;
        void getParameters(std::string& filePath);
        void createErrorName(std::string& houseFilePath);
    public:
        House();
        House(std::string& filePath);
        std::size_t getMaxSteps() const;
        std::size_t getMaxBattery() const;
        int getAmountOfDirt() const;
        std::pair<int, int> getDockingStationLocation() const;
        int getDirtLevel() const;
        std::size_t getTotalAmountOfSteps() const;
        float getCurentBatterySteps() const;

        void updateLocation(Step step);
        bool inDockingStation() const;
        void makeStep(Step step);
        bool isWall(Direction d) const;
        std::string getHouseName() const;
        // For simulation
        std::vector<std::vector<int>> getHouseSurface() const;
};

#endif  // HOUSE_H