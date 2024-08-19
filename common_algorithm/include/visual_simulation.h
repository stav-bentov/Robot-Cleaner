#ifndef VISUAL_SIMULATION_H_
#define VISUAL_SIMULATION_H_

#include <vector>
#include <utility>
#include <iostream>
#include <map>
#include <string>
#include <cmath>
#include <thread>  // for std::this_thread::sleep_for
#include <chrono>  // for std::chrono::milliseconds
#include "visualisation_configuration_manager.h"
#include "common_enums.h"
#include "house.h"

class VisualSimulation {
    private:
        std::vector<std::vector<std::string>> map;
        std::vector<Step> steps;
        std::pair<int, int> robotLocation;
        std::pair<int, int> dockingStationLocation;

        Step currentStep;

        int maxBatterySteps;
        double batterySteps;
        int allowedNumberOfSteps;
        int amountOfDirt;
        bool firstRun;

        int numSteps;
        int dirtLeft;
        std::string status;

        std::map<Elements, std::string> infoRepMapping = {
            {Elements::Wall, "#"},
            {Elements::DockingStation, "D"},
            {Elements::Robot, "R"},
            {Elements::EmptyWall, " "}
        };

        std::map<std::string, std::string> dirtLevelMapping = {
            {"1", "0"},
            {"2", "1"},
            {"3", "2"},
            {"4", "3"},
            {"5", "4"},
            {"6", "5"},
            {"7", "6"},
            {"8", "7"},
            {"9", "8"},
        };

        std::map<Step, std::pair<int, int>> stepMap = {
            {Step::East, {0, 1}},
            {Step::West, {0, -1}},
            {Step::South, {1, 0}},
            {Step::North, {-1, 0}},
            {Step::Stay, {0, 0}}
        };

        std::map<Step, std::string> directionStringMap = {
                    {Step::East, ">"},
                    {Step::West, "<"},
                    {Step::South, "v"},
                    {Step::North, "^"},
                    {Step::Stay, "-"}
        };
        
        std::map<std::string, std::string> stringToCodeColor = {
                    {"Red", "\033[31m"},
                    {"Green", "\033[32m"},
                    {"Yellow", "\033[33m"},
                    {"Blue", "\033[34m"},
                    {"White", "\033[37m"},
                    {"Reset", "\033[0m"}
        };
        
        std::map<Elements, std::string> elementToCodeColor = {
            {Elements::Robot, "\033[0m"},
            {Elements::DockingStation, "\033[0m"},
            {Elements::Direction, "\033[0m"},
            {Elements::Wall, "\033[0m"}
        };

        void processInputFile(std::string& fileName);
        void processOutputFile(std::string& fileName);

        void loadParameters(House& h);
        void loadHouseMapping(House& h);
        
        std::string intToHouseElementString(int num);

        void charge();
        void clean();

        void updateMap();
        void updateMSteps();
        
        void clearMapArea(int height);
        void printHouse(bool printDir);
        std::string getColorCode(std::string colorStr);
        Step charTostep(char c);
        
    public:
        void startSimulation();
        void processFiles(std::string inputFileName, std::string outputFileName);
        void changeInfoRepMapping(VisualConfigurationManager configM);
        void changeColorMapping(VisualConfigurationManager configM);

};
#endif  // VISUAL_SIMULATION_H_