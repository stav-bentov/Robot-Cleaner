#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]){
    readParameters(argc, argv);
    loadHouseFiles();
    loadAlgorithmFiles();
    openAlgorithms();
    //createSimulators();
    runSimulations();
    closeAlgorithms();
    writeResultsToCsv();
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
void MainManager::loadHouseFiles(){
    loadFiles(housePath, housespath, ".house");
}

/*
    Load algorithms (.so) files from housePath
*/
void MainManager::loadAlgorithmFiles(){
    loadFiles(algoPath, algorithms, ".so");
}

/*
    For each .so algorithm (gets its path and name)- opens it using dlopen
*/
void MainManager::openAlgorithms() {
    for (std::string algo : algorithms) {
        const auto& prevSize = AlgorithmRegistrar::getAlgorithmRegistrar().end();
        void* handle = dlopen(algo.c_str(), RTLD_LAZY);
        if (!handle) {
            std::filesystem::path algoPath(algo);
            ErrorManager::checkForError(true, "ERROR: Unable to open error file: " + algo, algoPath.stem().string() + ".error");
        } else if (AlgorithmRegistrar::getAlgorithmRegistrar().end() == prevSize) {
            std::cout << "Error loading algorithm library: No Algorithm Registered!" << std::endl;
        } else {
            algorithmsHandle.push_back(handle);
        }
    }
}

void MainManager::runSimulations() {
    int algo_idx = 0;
    int house_idx = 0;
    scores.resize(algorithms.size(), std::vector<int>(housespath.size(), noResult));
    
    std::mutex runningThreadsMutex;
    std::mutex coutMutex;
    std::mutex dataMutex;
    std::condition_variable simulatiosCv;
    std::condition_variable simulatiosDataCv;

    simulators.resize(algorithms.size());

    std::vector<std::pair<int, int>> simulatorsCorrd;
    std::vector<bool> simulatorWasTaken;
    int numSimulators = 0;

    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        simulators[algo_idx].resize(housespath.size());

        for (const auto& housePath : housespath) {
            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            if (algorithm) {
                std::cout << "algo_idx = " << algo_idx << std::endl;
                std::cout << "house_idx = " << house_idx << std::endl;
                std::filesystem::path fsHousePath(housePath);
                housesNames.push_back(fsHousePath.stem().string());

                auto simulator = std::make_unique<MySimulator>();
                simulator->prepareSimulationEnvironment(housePath, algo.name());
                simulator->setAlgorithm(std::move(algorithm));

                simulators[algo_idx][house_idx] = std::move(simulator);
                simulatorsCorrd.push_back(std::make_pair(algo_idx, house_idx));
                simulatorWasTaken.push_back(false);
                numSimulators++;
            } else {
                ErrorManager::checkForError(true, "ERROR: Failed creating: " + algo.name(), algo.name() + ".error");
            }
            house_idx++;
        }
        house_idx = 0; // Reset house index for the next algorithm
        algo_idx++;
    }
    
    std::atomic<int> nextSimulatorIdx(0);
    while (nextSimulatorIdx < numSimulators) {
        std::unique_lock<std::mutex> lockRunning(runningThreadsMutex);
        simulatiosCv.wait(lockRunning, [this]{ return runningThreads < numThreads; });
        runningThreads++;
        
        std::unique_lock<std::mutex> lock(dataMutex);
        simulatiosDataCv.wait(lock, [this,  &simulatorWasTaken, &nextSimulatorIdx]{ return !simulatorWasTaken[nextSimulatorIdx]; });
        if (nextSimulatorIdx >= numSimulators) {
            break;
        }
        int algoIdx = simulatorsCorrd[nextSimulatorIdx].first;
        int houseIdx = simulatorsCorrd[nextSimulatorIdx].second;
        simulatorWasTaken[nextSimulatorIdx] = true;
        nextSimulatorIdx++;
        
        threads.emplace_back([this, &runningThreadsMutex, &simulatiosCv, &dataMutex, &simulatiosDataCv, &simulatorWasTaken, algoIdx, houseIdx]() {
            std::unique_ptr<MySimulator> simulator_ptr = std::move(simulators[algoIdx][houseIdx]);
            simulators[algoIdx][houseIdx].reset();  // Prevent dangling reference
            simulatiosDataCv.notify_one();

            try {
                simulator_ptr->run();
                if (!summaryOnly) {
                    simulator_ptr->setOutput();
                }
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
            
            {
                std::lock_guard<std::mutex> lock(runningThreadsMutex);
                runningThreads--;
            }
            simulatiosCv.notify_one();
        });

    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "join Thread [" << std::this_thread::get_id() << "]" << std::endl;
            }
            thread.join();
        }
    }
}

/*void MainManager::runSimulations() {

    int algo_idx = 0;
    int house_idx = 0;
    scores.resize(algorithms.size(), std::vector<int>(housespath.size(), noResult));
    //std::mutex algoMutex;  // Protect shared access to algo
    std::mutex runningThreadsMutex;
    std::mutex coutMutex;
    //std::mutex scores_mut;
    std::condition_variable simulatiosCv;

    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        for (auto& housePath : housespath) {
            std::filesystem::path fsHousePath(housePath);
            housesNames.push_back(fsHousePath.stem().string());
            std::cout << "Adding simulation for House: " << housePath << " with Algorithm: " << algo.name() << std::endl;
        
            try {
                std::unique_lock<std::mutex> lock(runningThreadsMutex);
                
                Logger::getInstance().log("before runningThreads = " + std::to_string(runningThreads) + " .\n", LogLevels::FILE);
                Logger::getInstance().log("before numThreads = " + std::to_string(numThreads) + " .\n", LogLevels::FILE);
                simulatiosCv.wait(lock, [this]{return runningThreads < numThreads;});
                runningThreads++;
                
                Logger::getInstance().log("after runningThreads = " + std::to_string(runningThreads) + " .\n", LogLevels::FILE);
                Logger::getInstance().log("after numThreads = " + std::to_string(numThreads) + " .\n", LogLevels::FILE);
                threads.emplace_back([this, &coutMutex, &runningThreadsMutex, &simulatiosCv, housePath, algo]() {
                    {
                        std::lock_guard<std::mutex> lock(coutMutex);
                        std::cout << "Running simulation in thread: " <<std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) <<
                                    "for house= "<< housePath << " with Algorithm: " << algo.name() << std::endl;
                    }
                    MySimulator simulator;
                    simulator.prepareSimulationEnvironment(housePath, algo.name());
                    // TODO: add if algorithm null ptr...
                    std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
                    simulator.setAlgorithm(std::move(algorithm));
                    simulator.run();

                    if (!summaryOnly) {
                        simulator.setOutput();
                    }

                    //{
                    //    std::lock_guard<std::mutex> lock(scores_mut);
                    //    scores[algo_idx][house_idx] = simulator.getScore();
                    //}
                {
                    std::lock_guard<std::mutex> lock(runningThreadsMutex);
                    runningThreads--;
                }
                simulatiosCv.notify_one();
            });
                
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
            house_idx++;
        }
        algo_idx++;
    }
    // Join all threads to ensure they complete before exiting the function
    for (auto& thread : threads) {
        if (thread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "join Thread [" << std::this_thread::get_id() << "]" << std::endl;
            }
            thread.join();
        }
    }
}*/


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
    // TODO: Add this!
    /*CsvManager csvManager(algorithmNames, housesNames, scores);
    csvManager.writeResultsToCsv();*/
}



/*void MainManager::runSimulations() {
    std::cout << "numThreads = " << numThreads << std::endl;
    std::mutex runningThreadsMutex;
    std::mutex coutMutex;
    std::condition_variable simulatiosCv;

    for (int algo_idx = 0; algo_idx < static_cast<int>(simulators.size()); algo_idx++) {
        auto& simulatorsForAlgorithm = simulators[algo_idx];
        for (int house_idx = 0; house_idx < static_cast<int>(simulatorsForAlgorithm.size()); house_idx++) {
            auto& simulator = simulatorsForAlgorithm[house_idx];

            try {
                std::unique_lock<std::mutex> lock(runningThreadsMutex);
                simulatiosCv.wait(lock, [this]{ return runningThreads < numThreads; });
                runningThreads++;
                
                threads.emplace_back(&MainManager::threadWork, this, std::ref(*simulator), std::ref(coutMutex), std::ref(runningThreadsMutex), std::ref(simulatiosCv), house_idx, algo_idx);
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
    }
    for (auto& thread : threads) {
        if (thread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "join Thread [" << std::this_thread::get_id() << "]" << std::endl;
            }
            thread.join();
        }
    }
}*/


void MainManager::threadWork(MySimulator& simulator, std::mutex& coutMutex, std::mutex& runningThreadsMutex,std::condition_variable& simulatiosCv, int house_idx, int algo_idx) {
    {
        std::lock_guard<std::mutex> lock(coutMutex);

        Logger::getInstance().log("Running simulation in thread: " +std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()))+"house_idx = "+std::to_string(house_idx) +"for house: " +
        housespath[house_idx]+"algo_idx = "+std::to_string(algo_idx)+ " for algorithm " + algorithms[algo_idx] +".\n", LogLevels::FILE);
    }
    simulator.run();  // This will now work

    {
        std::lock_guard<std::mutex> lock(runningThreadsMutex);
        runningThreads--;
    }
    simulatiosCv.notify_one();
} 

/*void MainManager::runSimulations() {
    for (std::vector<MySimulator> simulatorsForAlgorithm: simulators) {
        for (MySimulator simolator: simulatorsForAlgorithm) {
            try {
                std::unique_lock<std::mutex> lock(runningThreadsMutex);

                Logger::getInstance().log("runningThreads = " + std::to_string(runningThreads) + " .\n", LogLevels::FILE);
                Logger::getInstance().log("numThreads = " + std::to_string(numThreads) + " .\n", LogLevels::FILE);

                simulatiosCv.wait(lock, [this]{return runningThreads < numThreads;});
                runningThreads++;
                threads.emplace_back([=, this, simulator = std::move(simolator)]() mutable {
                    try {
                        simulator.run();

                        if (summaryOnly) {
                            simulator.setOutput();
                        }

                        //{
                        //    std::lock_guard<std::mutex> lock(scores_mut);
                        //    scores[algo_idx][house_idx] = simulator.getScore();
                        //

                    } catch (const std::exception& e) {
                        std::lock_guard<std::mutex> lock(cerr_mut);
                        //std::string errorMessage = "Exception in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +
                        //                            "]: " + e.what() + " for algorithm: " + algo.name() + " and house: " + housePath;
                        //
                       std::string errorMessage = "1 Exception in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +
                                                    "]: ";
                        Logger::getInstance().log(errorMessage, LogLevels::FILE);                        
                        std::cout << "Exception in thread [" << std::this_thread::get_id() << "]: " << e.what() << std::endl;
                    } catch (...) {
                        std::lock_guard<std::mutex> lock(cerr_mut);
                        //std::string errorMessage = "Exception in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +
                        //                            "]: for algorithm: " + algo.name() + " and house: " + housePath;
                        std::string errorMessage = "2 Exception in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +
                                                    "]: ";
                        Logger::getInstance().log(errorMessage, LogLevels::FILE);  
                        std::cout << "Unknown exception in thread [" << std::this_thread::get_id() << "]" << std::endl;
                    }

                    {
                        std::lock_guard<std::mutex> lock(runningThreadsMutex);
                        runningThreads--;
                    }
                    simulatiosCv.notify_one();
                });
                    
                } catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << std::endl;
                }
        }
    }
}*/


/*void MainManager::threadWork(AbstractAlgorithm& algorithm, std::string housePath, int algo_idx, int house_idx, std::string algorithmName) {
    {
        std::lock_guard<std::mutex> lock(cerr_mut);
        std::cout << "Thread [" << std::this_thread::get_id() << "] running simulation" << std::endl;
    }

    MySimulator simulator;
    simulator.prepareSimulationEnvironment(housePath, algorithmName);
    simulator.setAlgorithm(algorithm);
    simulator.run();

    if (summaryOnly) {
        simulator.setOutput();
    }

    {
        std::lock_guard<std::mutex> lock(scores_mut);
        scores[algo_idx][house_idx] = simulator.getScore();
    }

    {
        std::lock_guard<std::mutex> lock(runningThreadsMutex);
        runningThreads--;
    }

    simulatiosCv.notify_one();
}*/

/*void MainManager::createSimulators() {
    int algo_idx = 0;
    int house_idx = 0;
    simulators.resize(algorithms.size());

    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        simulators[algo_idx].resize(housespath.size());

        for (const auto& housePath : housespath) {
            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            if (algorithm) {
                std::cout << "algo_idx = " << algo_idx << std::endl;
                std::cout << "house_idx = " << house_idx << std::endl;
                std::filesystem::path fsHousePath(housePath);
                housesNames.push_back(fsHousePath.stem().string());

                auto simulator = std::make_unique<MySimulator>();
                simulator->prepareSimulationEnvironment(housePath, algo.name());
                simulator->setAlgorithm(std::move(algorithm));

                simulators[algo_idx][house_idx] = std::move(simulator);
            } else {
                ErrorManager::checkForError(true, "ERROR: Failed creating: " + algo.name(), algo.name() + ".error");
            }
            house_idx++;
        }
        house_idx = 0; // Reset house index for the next algorithm
        algo_idx++;
    }
}*/