#ifndef OUTPUT_MANAGER_H_
#define OUTPUT_MANAGER_H_

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "common_enums.h"
#include "visual_simulation.h"

class OutputManager {
    private:
        ConfigurationManager configM;
        std::string houseFilePath;
        std::string algorithmName;
        std::string outputName;
        std::vector<Step> stepsMade;
        int amountOfSteps;
        int amountOfDirtLeft;
        std::string status;

        void createOuputName();   
        std::string getChar(Step s);
    public:
        OutputManager(std::string housePath, std::string algoName);
        void writeOutput(std::vector<Step> steps, std::size_t numSteps, int amountOfDirtLeft, std::string status, bool inDocking, int score);
        void displaySim();
};
#endif  // OUTPUT_MANAGER_H_