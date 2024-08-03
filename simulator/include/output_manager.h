#ifndef OUTPUT_MANAGER_H_
#define OUTPUT_MANAGER_H_

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include "enums.h"
#include "common.h"
#include "visual_simulation.h"

class OutputManager {
    private:
        ConfigurationManager configM;
        std::string inputName;
        std::string outputName;
        std::vector<Step> stepsMade;
        int amountOfSteps;
        int amountOfDirtLeft;
        std::string status;

        void createOuputName();   
        std::string getChar(Step s);
    public:
        OutputManager();
        void writeOutput(std::vector<Step> steps, std::size_t numSteps, int amountOfDirtLeft, std::string status);
        void setInputName(std::string& inputFileName);     
        void displaySim();
};
#endif  // OUTPUT_MANAGER_H_