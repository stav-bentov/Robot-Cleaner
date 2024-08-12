#ifndef MAIN_MANAGER_H
#define MAIN_MANAGER_H

#include "../../common/AlgorithmRegistrar.h"
#include "../../common_algorithm/include/common_enums.h"
#include "my_simulator.h"
#include <filesystem>
#include <dlfcn.h>

class MainManager {
    public:
        void run(int argc, char* argv[]);
    private:
        std::string housePath;
        std::string algoPath;
        int numThread;
        bool summaryOnly;

        MainManager() : housePath("."), algoPath("."), numThread(10), summaryOnly(false) {};
        void loadFiles(std::string& path, std::vector<std::string>& container, std::string extension);
        void loadHouseFiles(std::string& housePath, std::vector<std::string>& houses);
        void loadAlgorithmFiles(std::string& algoPath, std::vector<std::string>& algorithms);
        void openAlgorithms(std::vector<std::string>& algorithms, std::vector<void*> algorithmsHandle);
        void readParameters(int argc, char* argv[], std::string& housePath, std::string& algoPath);
        void runSimulations(std::vector<std::string>& houses);
        void closeAlgorithms(std::vector<void*> algorithmsHandle);
};

#endif  // MAIN_MANAGER_H