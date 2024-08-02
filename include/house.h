#ifndef HOUSE_H
#define HOUSE_H

#include <vector>
#include <string>
#include <map>
#include <array>
#include <fstream> 
#include <iostream> 
#include <sstream>
#include <memory>
#include <cstddef>
#include <algorithm>  
#include <stdexcept>
#include <type_traits>
#include "wall_sensor.h"
#include "common.h"

class House {
    private:
        std::string houseName;
        std::size_t maxSteps;
        std::size_t maxBattery;
        int amountOfDirt;
        int rows;
        int cols;
        std::pair<int, int> dockingStationLocation; 

        // Changed throgh the
        double currentBatterySteps;
        std::pair<int, int> currentLocation; 
        std::size_t totalAmountOfSteps;
        std::vector<std::vector<int>> houseSurface;
        
        void processParametersFromFile(std::ifstream& file);
        void processHouseMappingFromFile(std::ifstream& file);
        void logHouseSurface();
        void charge();
        void discharge();
        void clean();
        bool isWall(std::pair<int, int> location) const;
        
        template<typename T>
        void processVariable(std::ifstream& file, T& var, const std::string& errorMessage);
    public:
        static const std::map<Step, std::pair<int, int>> stepMap;
        static const std::map<Direction, std::pair<int, int>> directionMap;

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
        bool enoughSteps() const;
        bool isWall(Direction d) const;

        // For simulation
        std::vector<std::vector<int>> getHouseSurface() const;
};

#endif  // HOUSE_H