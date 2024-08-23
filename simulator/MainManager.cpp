#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]) {
    readParameters(argc, argv);
    calcTimeout();
    loadHouseFiles();
    createHouses();
    loadAlgorithmFiles();
    openAlgorithms();
    manageTasks(); 
    closeAlgorithms();
    writeResultsToCsv();
}


void MainManager::calcTimeout() {
    SimConfigurationManager simConfigManager;
    milisecondPerStep = simConfigManager.getTimePerStep();
  //  std::cout <<" milisecond Per Step: " << milisecondPerStep << " milliseconds" << std::endl;
}

/*
    Reads parameters given in command line
*/
void MainManager::readParameters(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        // Check if argument starts with this sign
        if (arg.rfind("-house_path=", 0) == 0) {
            // "you are not required to have support for spaces around the equal sign"
            housePath = arg.substr(12);
        } else if (arg.rfind("-algo_path=", 0) == 0) {
            // "you are not required to have support for spaces around the equal sign"
            algoPath = arg.substr(11);
        } else if (arg.rfind("-num_threads=", 0) == 0) {
            try {
                numThreads = std::stoi(arg.substr(13));
                if (numThreads <= 0) {
                    throw std::runtime_error("Invalid number of threads provided. Using default: 10.");
                }
            } catch (const std::exception& e) {
                std::cerr << "Invalid number of threads provided. Using default: 10." << std::endl;
                numThreads = 10;
            }
        } else if (arg.rfind("-summary_only", 0) == 0) {
            summaryOnly = true;    
        }
    }
}

/*
    Gets path to laod files with of type extension and push to container
*/
void MainManager::loadFiles(std::string& path, std::vector<std::string>& container, std::string extension) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == extension) {
            container.push_back(entry.path().string());
        }
    }
}

/*
    Load houses (.house) files from housePath
*/
void MainManager::loadHouseFiles() {
    loadFiles(housePath, housespath, ".house");
}

void MainManager::createHouses() {
    for (const auto& housePath : housespath) {
        try {
            auto sharedHouse = std::make_shared<House>(housePath);
            houses.emplace_back(sharedHouse);
            housesNames.push_back(sharedHouse->getHouseName());
        }
        catch (const std::exception& e) {
          //  std::cout << "ERROR: Failed creating house: " << housePath << " Exception: " << e.what() << std::endl;
            continue;
        }
    }
}

/*
    Load algorithms (.so) files from housePath
*/
void MainManager::loadAlgorithmFiles(){
    loadFiles(algoPath, algorithmsPath, ".so");
}

/*
    For each .so algorithm (gets its path and name)- opens it using dlopen
*/
void MainManager::openAlgorithms() {
    for (std::string algo : algorithmsPath) {
        const auto& prevSize = AlgorithmRegistrar::getAlgorithmRegistrar().end();
        void* handle = dlopen(algo.c_str(), RTLD_LAZY);
        if (!handle) {
            std::filesystem::path algoPath(algo);
            ErrorManager::checkForError(true, "ERROR: Unable to open error file: " + algo, algoPath.stem().string() + ".error");
        } else if (AlgorithmRegistrar::getAlgorithmRegistrar().end() == prevSize) {
          //  std::cout << "Error loading algorithm library: No Algorithm Registered!" << std::endl;
        } else {
            algorithmsHandle.push_back(handle);
        }
    }
}

void MainManager::createTasks(std::list<Task>& tasks, boost::asio::io_context& ioContext, std::latch& workDone, 
                            std::shared_ptr<int> runningThreads, std::mutex& runningThreadsMutex, std::shared_ptr<std::condition_variable> simulatiosCv) {
    int algoIdx = 0;
    int houseIdx = 0;
    bool validAlgo = true;

    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        for (std::shared_ptr<House> house: houses) {
            if (!validAlgo) {
                scores[algoIdx][houseIdx] = invalid;
                continue;
            }

            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            if (!algorithm) {
                validAlgo = false;
                scores[algoIdx][houseIdx] = invalid;
                ErrorManager::checkForError(true, "ERROR: Failed creating: " + algo.name(), algo.name() + ".error");
                continue;
            } 

            std::shared_ptr<House> houseCopy = std::make_shared<House>(*house);
            std::cerr <<"Added task:" <<algo.name() <<" house:"<< houseCopy->getHouseName()<<std::endl;
            std::cerr <<"details: algoIdx =" <<algoIdx <<" houseIdx= "<< houseIdx <<std::endl;
            tasks.emplace_back(std::move(algorithm), houseCopy, algoIdx, houseIdx, algo.name(), ioContext, 
                                workDone, summaryOnly, milisecondPerStep, runningThreads, runningThreadsMutex, simulatiosCv);
            houseIdx++;
        }
        algoIdx++;
        validAlgo = true;
        houseIdx = 0;
    }
    std::cerr<< "task.size = " <<tasks.size()<< std::endl;
}

void MainManager::runTasks(std::list<Task>& tasks, std::shared_ptr<int> runningThreads, 
                            std::mutex& runningThreadsMutex, std::shared_ptr<std::condition_variable> simulatiosCv) {
    for (auto& task : tasks) {
        bool notified = false;
        std::unique_lock<std::mutex> lockRunning(runningThreadsMutex);
        std::cerr<< "in lockRunning for " <<task.getAlgoIdx()<< ", " << task.getHouseIdx()<< std::endl;
        
        while (*runningThreads >= numThreads) {
            // Waiting for the condition variable with a timeout to avoid infinite waiting
            if (simulatiosCv->wait_for(lockRunning, std::chrono::milliseconds(100)) == std::cv_status::no_timeout) {
                notified = true;
                break;
            }
            // Periodic sleep to avoid busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        if (!notified) {
            std::cerr << "Thread woke up after timeout due to runningThreads = " << *runningThreads << std::endl;
        }
        
        //simulatiosCv->wait(lockRunning, [this, runningThreads]{ return *runningThreads < numThreads; });
        std::cerr<< "Passed CD- for run " <<task.getAlgoIdx() << ", " << task.getHouseIdx()<<" runningThreads ="<<*runningThreads<< std::endl;
        (*runningThreads)++;
        task.run();
    }
    // Wait on latch, for all threads to report done
    std::cerr<< "Stuck in wait" << std::endl;

}


void MainManager::writeOutputFiles(std::list<Task>& tasks) {
    std::cerr<< "writeOutputFiles" << std::endl;
    size_t taskIndex = 0;
    if (!summaryOnly) {
        for (auto& task: tasks) {
            task.setOutput();
            scores[task.getAlgoIdx()][task.getHouseIdx()] = task.getScore();
            ++taskIndex;
            task.detach();
        }
    }
}

void MainManager::manageTasks() {
    boost::asio::io_context ioContext;
    std::mutex runningThreadsMutex;
    std::shared_ptr<std::condition_variable> simulatiosCv = std::make_shared<std::condition_variable>();
    std::shared_ptr<int> runningThreads = std::make_shared<int>(0);

    // Prevent the I/O context from stopping until all work is done.
    // Even if there is no active timer to wait for
    auto workGuard = boost::asio::make_work_guard(ioContext);
    
    // Run io_context in a separate thread
    std::jthread ioThread([&ioContext]() {
        ioContext.run();
    });

    const long numberOfSimulations = algorithmsHandle.size()*houses.size();
    std::latch workDone(numberOfSimulations);

    scores.resize(algorithmsHandle.size(), std::vector<int>(houses.size(), noResult));

    std::list<Task> tasks;
    createTasks(tasks, ioContext, workDone, runningThreads, runningThreadsMutex, simulatiosCv);

    runTasks(tasks, runningThreads, runningThreadsMutex, simulatiosCv);
    workDone.wait();

    // stop the io_context
    ioContext.stop();
    
    writeOutputFiles(tasks);
}

void MainManager::closeAlgorithms() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    for (void* handle : algorithmsHandle) {
        if (handle) {
            dlclose(handle);
        }
    }    
    algorithmsHandle.clear();
}


void MainManager::writeResultsToCsv() {
    try {
        CsvManager csvManager(algorithmNames, housesNames, scores);
        csvManager.writeResultsToCsv();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
