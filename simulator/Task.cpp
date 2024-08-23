#include "include/task.h"

std::mutex Task::cerrMutex; // Definition
void Task::timerHandler(const boost::system::error_code& ec, Task& task, std::chrono::time_point<std::chrono::system_clock> start, pthread_t threadHandler) {
    // Succeeed- up to time!
    if (ec == boost::asio::error::operation_aborted) {
        {
          std::lock_guard<std::mutex> lock(cerrMutex);
          std::cerr << "Task::timerHandler: Timer for task " << task.algoIdx << ", " << task.houseIdx << " was canceled" << std::endl;
        }
        
    } else if (!ec) {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
        // print duration that passed
        // Only one of the timer or the task's completion writes the result and signals the latch.
        bool expected = false;
        bool success = task.guard.compare_exchange_strong(expected, true);
        if(!success) {
            // Timer already expired and stopped
            {
              std::lock_guard<std::mutex> lock(cerrMutex);
              std::cerr << "Task::timerHandler: Task " << task.algoIdx << ", " << task.houseIdx << ", already wrote, before timer expiration" << std::endl;
            }
        }
        else {
            // send a cancel request to the thread that seems to be stuck
            {
              std::lock_guard<std::mutex> lock(cerrMutex);
              std::cerr << "Timer for task " << task.algoIdx << ", " << task.houseIdx << " expired after " << duration.count() <<"finished task because time ran out"<< std::endl;
            }
            task.threadComplete();
            pthread_cancel(threadHandler);
        }
    }
}

void Task::threadComplete() {
    {
      std::lock_guard<std::mutex> lock(cerrMutex);
      std::cerr << "Task::threadComplete for task " << algoIdx << ", " << houseIdx  << std::endl;
    }
    {
        std::lock_guard<std::mutex> lockRunning(runningThreadsMutex);
        (*runningThreads)--;
    }
    simulatiosCv->notify_all();
    workDone.count_down();
}

void Task::calcTimeout() {
	maxSteps = house->getMaxSteps();
  timeout = maxSteps*milisecondPerStep;
}


void Task::run() {
    {
      std::lock_guard<std::mutex> lock(cerrMutex);
      std::cerr <<"Task::run algoIdx " << algoIdx << " with houseIdx: "<< houseIdx << std::endl;
    }

    calcTimeout();
    errorInRun = false;

    // Set simulator environment
    try {
        simulator.prepareSimulationEnvironment(house, houseFilePath, algoName);
    } 
    catch (const std::exception& e) {
        std::cerr << "ERROR: Failed creating house: " << houseFilePath << "with algo: " << algoIdx << " Exception: " << e.what() << std::endl;
        errorInRun = true;
        threadComplete();
        return;
    }

    std::cerr <<" Task::taskWork simulator.setAlgorithm " << algoIdx << ", "<< houseIdx << std::endl;
    // Set algorithm in simulator
    simulator.setAlgorithm(std::move(algorithm));

    myThread = std::jthread([this] {
        try {
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

          simulator.run();
          
          // After the task was done, or there was a timeout
          timer.cancel();

          // There might be a race condition
          // Ensures there are no race conditions between the task finishing and the timer firing
          bool expected = false;
          bool success = guard.compare_exchange_strong(expected, true);
          if(!success) {
              // The timer expired and wrote
              {
                std::lock_guard<std::mutex> lock(cerrMutex);
                std::cerr <<"Timer already expired and wrote: " << algoIdx << ", "<< houseIdx << std::endl;
              }
          }
          else {
            simFinished = true;
            threadComplete();
            {
                std::lock_guard<std::mutex> lock(cerrMutex);
                std::cerr <<"Finished naturally: " << algoIdx << ", "<< houseIdx << std::endl;
            }
          }
        }
        catch (const std::exception& e) {
          ErrorManager::checkForError(true, "ERROR: Failed running: " + algoName + "with house: " + house->getHouseName(), algoName + ".error");
          std::cerr << "Error: " << e.what() << std::endl;
          errorInRun = true;
          threadComplete();
        }
    });
}

int Task::getScore() const {
    return score;
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

void Task::setOutput() {
    // If simulation didnot finished on time- set score to be the initial score...
    if (simFinished && !errorInRun) {
        std::cerr << "Simulator for: " << algoName << ", " << house->getHouseName() << " ended fully" << std::endl;
        simulator.calculateScore();
    }
    else {
      std::cerr << "Simulator for: " << algoName << ", " << house->getHouseName() << " was stopped" << std::endl;
    }
    score = simulator.getScore();
    simulator.setOutput();
}