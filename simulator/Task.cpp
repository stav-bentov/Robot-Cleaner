#include "include/task.h"

void Task::timerHandler(const boost::system::error_code& ec, Task& task, std::chrono::time_point<std::chrono::system_clock> start, pthread_t threadHandler) {
    // Succeeed- up to time!
    if (ec == boost::asio::error::operation_aborted) {
        if (!task.errorInRun) {
            task.score = task.simulator.getScore();
            if (!task.summaryOnly) {
                task.simulator.setOutput();
            }
            std::cout << "No error in run, score is:" << task.score << std::endl;
        }
        std::cout << "Timer for task " << task.algoIdx << ", " << task.houseIdx << " was canceled" << std::endl;
    } else if (!ec) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        // print duration that passed
        std::cout << "Timer for task " << task.algoIdx << ", " << task.houseIdx << " expired after " << duration.count() << std::endl;

        // Only one of the timer or the task's completion writes the result and signals the latch.
        bool expected = false;
        bool success = task.guard.compare_exchange_strong(expected, true);
        if(!success) {
            // the task already wrote
            std::cout << "Task " << task.algoIdx << ", " << task.houseIdx << ", already wrote, before timer expiration" << std::endl;
        }
        else {
            // send a cancel request to the thread that seems to be stuck
            pthread_cancel(threadHandler);
            task.workDone.count_down();
        }
    }
}

void Task::calculateScore() {
    score = maxSteps * 2 + house->getAmountOfDirt() * 300 + 2000;
}

void Task::calcTimeout() {
	maxSteps = house->getMaxSteps();
    timeout = maxSteps*milisecondPerStep;
}


void Task::run() {
    calcTimeout();
    calculateScore();
    errorInRun = false;
    myThread = std::jthread([this] {
        // Allow thread to be canceled asyncronously from another thread
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);            
        
        // Set the timer
        boost::asio::steady_timer timer(ioContext, std::chrono::milliseconds(timeout));
        auto curr_time = std::chrono::system_clock::now();
        auto threadHandler = pthread_self();

        //The timer is set to wait asynchronously using async_wait. 
        // If the timer expires before the task finishes, it triggers the timerHandler.
        timer.async_wait([&](const boost::system::error_code& ec) {
            timerHandler(ec, *this, curr_time, threadHandler);
        });

        taskWork();

        // After the task was done, or there was a timeout
        timer.cancel();

        // There might be a race condition
        // Ensures there are no race conditions between the task finishing and the timer firing
        bool expected = false;
        bool success = guard.compare_exchange_strong(expected, true);
        if(!success) {
            // the timer expired and wrote
            std::cout << "Task " << algoIdx << ", " << houseIdx << ", timer already expired and wrote" << std::endl;
        }
        else {
            std::cout << "Task " << algoIdx << ", " << houseIdx << ", finished and wrote " << std::endl;
            workDone.count_down();
        }
    });
}

void Task::taskWork() {
    // Set simulator environment
    try {
        simulator.prepareSimulationEnvironment(house, houseFilePath, algoName);
    } 
    catch (const std::exception& e) {
        std::cout << "ERROR: Failed creating house: " << houseFilePath << "with algo: " << algoName << " Exception: " << e.what() << std::endl;
        return;
    }

    // Set algorithm in simulator
    simulator.setAlgorithm(std::move(algorithm));

    // run simulator- catch an error
    try {
        simulator.run();
    } catch (const std::exception& e) {
        ErrorManager::checkForError(true, "ERROR: Failed running: " + algoName + "with house: " + houseFilePath, algoName + ".error");
        std::cout << "Error: " << e.what() << std::endl;
        errorInRun = true;
    }

    {
        std::lock_guard<std::mutex> lockRunning(runningThreadsMutex);
        runningThreads--;
    }

    simulatiosCv.notify_one();
}

int Task::getScore() const {
    return score;
}

void Task::join() {
    if(myThread.joinable()) {
        myThread.join();
    }
}

void Task::detach() {
    if(myThread.joinable()) {
        myThread.detach();
    }
}


std::string Task::getHouseName() const {
    return house->getHouseName();
}

std::string Task::getAlgoName() const {
    return algoName;
}

int Task::getAlgoIdx() const {
    return algoIdx;
}


int Task::getHouseIdx() const {
    return houseIdx;
}