#ifndef MAIN_MANAGER_H
#define MAIN_MANAGER_H

#include "../../common/AlgorithmRegistrar.h"
#include "../../common_algorithm/include/common_enums.h"
#include "my_simulator.h"
#include "task.h"
#include "csv_manager.h"
#include <filesystem>
#include <dlfcn.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <list>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class MainManager {
    public:
        MainManager() : housePath("."), algoPath("."), numThreads(10), summaryOnly(false), noResult(-1), invalid(-2), runningThreads(0), milisecondPerStep(100) {};
        void run(int argc, char* argv[]);
        
    private:
        std::string housePath;
        std::string algoPath;
        int numThreads;
        bool summaryOnly;
        std::vector<std::string> housespath;
        std::vector<std::string> algorithmsPath;
        std::vector<std::shared_ptr<House>> houses;
        std::vector<void*> algorithmsHandle;
        std::vector<std::vector<int>> scores;
        std::vector<std::vector<std::unique_ptr<MySimulator>>> simulators;
        const int noResult;
        const int invalid;
        std::vector<std::string> algorithmNames;
        std::vector<std::string> housesNames;
        std::vector<std::pair<int, int>> simulatorsCorrd;
        std::vector<bool> simulatorWasTaken;
        int numSimulators;
        int runningThreads;
        std::vector<std::thread> threads;
        int milisecondPerStep;

        void calcTimeout();
        void loadFiles(std::string& path, std::vector<std::string>& container, std::string extension);
        void loadHouseFiles();
        void createHouses();
        void loadAlgorithmFiles();
        void openAlgorithms();
        void readParameters(int argc, char* argv[]);
        void closeAlgorithms();
        void writeResultsToCsv();
        void runTasks(boost::asio::io_context& ioContext);
        void manageTasks();
};

#endif  // MAIN_MANAGER_H