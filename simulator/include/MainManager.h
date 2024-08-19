#ifndef MAIN_MANAGER_H
#define MAIN_MANAGER_H

#include "../../common/AlgorithmRegistrar.h"
#include "../../common_algorithm/include/common_enums.h"
#include "my_simulator.h"
#include "csv_manager.h"
#include <filesystem>
#include <dlfcn.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "../include/sim_config_manager.h"

class MainManager {
    public:
        MainManager() : housePath("."), algoPath("."), numThreads(10), summaryOnly(false), noResult(-1), runningThreads(0) {};
        void run(int argc, char* argv[]);
    private:
        std::string housePath;
        std::string algoPath;
        int numThreads;
        bool summaryOnly;
        std::vector<std::string> housespath;
        std::vector<std::string> algorithms;
        std::vector<void*> algorithmsHandle;
        std::vector<std::vector<int>> scores;
        const int noResult;
        std::vector<std::string> algorithmNames;
        std::vector<std::string> housesNames;

        std::mutex runningThreadsMutex;
        std::condition_variable simulatiosCv;
        int runningThreads;
        std::vector<std::thread> threads;
        int maxSteps;

        void loadFiles(std::string& path, std::vector<std::string>& container, std::string extension);
        void loadHouseFiles();
        void loadAlgorithmFiles();
        void getMaxSteps();
        void openAlgorithms();
        void readParameters(int argc, char* argv[]);
        void createSimulations();
        void closeAlgorithms();
        void writeResultsToCsv();
};

#endif  // MAIN_MANAGER_H