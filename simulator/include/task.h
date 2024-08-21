#include <boost/asio.hpp>
#include <latch>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../common/AbstractAlgorithm.h"
#include "../../common_algorithm/include/house.h"
#include "my_simulator.h"

class Task {
    private:
        // Set in constructor
        std::unique_ptr<AbstractAlgorithm> algorithm;
        std::shared_ptr<House> house;
        const int algoIdx; 
        const int houseIdx; 
        std::string algoName;
        boost::asio::io_context& ioContext;
        std::latch& workDone; // will follow ending of multipule tasks
        int score;
        bool summaryOnly;
        int milisecondPerStep;
        int& runningThreads;
        std::mutex& runningThreadsMutex;
        std::condition_variable& simulatiosCv;
        std::string houseFilePath;
        MySimulator simulator;


        // Set in run
        std::atomic<bool> guard;
        std::jthread myThread;
        int maxSteps;
        // task that doesn't finish on time shall got -1
        // otherwise, shall get the time in took to finish
        int timeout;
        int initialDirt;
        bool errorInRun;
        
        // Private function for the timer handler
        static void timerHandler(const boost::system::error_code& ec, Task& task, std::chrono::time_point<std::chrono::system_clock> start, pthread_t threadHandler);
        void calcTimeout();
        void calculateScore();
    public:
        Task(std::unique_ptr<AbstractAlgorithm> algorithm, std::shared_ptr<House> house, int algoIdx, int houseIdx, 
                std::string algoName, boost::asio::io_context& ioContext, std::latch& workDone,
                bool& summaryOnly, int milisecondPerStep, int& runningThreads, std::mutex& runningThreadsMutex, 
                std::condition_variable& simulatiosCv)
            : algorithm(std::move(algorithm)), house(house), algoIdx(algoIdx), houseIdx(houseIdx), algoName(algoName), 
                ioContext(ioContext), workDone(workDone), score(-200), summaryOnly(summaryOnly), milisecondPerStep(milisecondPerStep),
                runningThreads(runningThreads), runningThreadsMutex(runningThreadsMutex), simulatiosCv(simulatiosCv), houseFilePath(house->getHouseFilePath()) {}
        void taskWork();
        void run();
        int getScore() const;
        std::string getHouseName() const;
        std::string getAlgoName() const;
        void join();
        void detach();
        int getAlgoIdx() const;
        int getHouseIdx() const;

};