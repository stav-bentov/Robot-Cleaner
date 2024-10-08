#include <boost/asio.hpp>
#include <latch>
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "../../common/AbstractAlgorithm.h"
#include "../../common_algorithm/include/house.h"
#include "my_simulator.h"
#include <semaphore>

class Task {
    private:
        // Set in constructor
        std::unique_ptr<AbstractAlgorithm> algorithm;
        std::shared_ptr<House> house;
        const int algoIdx; 
        const int houseIdx; 
        std::string algoName;
        boost::asio::io_context& ioContext;
        std::latch& workDone; // Will follow ending of multipule tasks
        int score;
        bool summaryOnly;
        int milisecondPerStep;
        std::counting_semaphore<>& semaphore;

        std::string houseFilePath;
        bool simFinished;

        // Set in run
        MySimulator simulator;
        std::atomic<bool> guard;
        std::jthread myThread;
        int maxSteps;
        int timeout;
        int initialDirt;
        bool errorInRun;
        
        // Private function for the timer handler
        static void timerHandler(const boost::system::error_code& ec, Task& task, std::chrono::time_point<std::chrono::system_clock> start, pthread_t threadHandler);
        void calcTimeout();
        void threadComplete();
    public:
    static std::mutex cerrMutex; // Declaration
        Task(std::unique_ptr<AbstractAlgorithm> algorithm, std::shared_ptr<House> house, int algoIdx, int houseIdx, 
                std::string algoName, boost::asio::io_context& ioContext, std::latch& workDone,
                bool& summaryOnly, int milisecondPerStep, std::counting_semaphore<>& _semaphore)
            : algorithm(std::move(algorithm)), house(house), algoIdx(algoIdx), houseIdx(houseIdx), algoName(algoName), 
                ioContext(ioContext), workDone(workDone), score(-200), summaryOnly(summaryOnly), milisecondPerStep(milisecondPerStep),
                semaphore(_semaphore), houseFilePath(house->getHouseFilePath()), timeout(200) {}
        void run();
        int getScore() const;
        std::string getHouseName() const;
        std::string getAlgoName() const;
        void detach();
        int getAlgoIdx() const;
        int getHouseIdx() const;
        void setOutputAndCalcScore(bool withOutputFile);

};