#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]){
    readParameters(argc, argv);
    loadHouseFiles();
    loadAlgorithmFiles();
    openAlgorithms();
    createSimulators();
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

void MainManager::createSimulators() {
    int algo_idx = 0;
    int house_idx = 0;

    simulators.resize(algorithms.size());
    numSimulators = 0;

    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        simulators[algo_idx].resize(housespath.size());
        for (const auto& housePath : housespath) {
            try {
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
            catch (const std::exception& e) {
                std::cout << "ERROR: Failed creating house: " << housePath << "with algo: " << algo.name() << " Exception: " << e.what() << std::endl;
                continue;
            }
        }
        simulators[algo_idx].resize(house_idx);
        house_idx = 0; // Reset house index for the next algorithm
        algo_idx++;
    }
    simulators.resize(algo_idx);
}

void MainManager::runSimulations() {
    std::mutex runningThreadsMutex;
    std::mutex coutMutex;
    std::mutex dataMutex;
    std::mutex scoreMutex;
    std::condition_variable simulatiosCv;
    std::condition_variable simulatiosDataCv;

    scores.resize(algorithms.size(), std::vector<int>(housespath.size(), noResult));
    std::atomic<int> nextSimulatorIdx(0);

    while (nextSimulatorIdx < numSimulators) {
        std::unique_lock<std::mutex> lockRunning(runningThreadsMutex);
        simulatiosCv.wait(lockRunning, [this]{ return runningThreads < numThreads; });
        runningThreads++;
        
        std::unique_lock<std::mutex> lock(dataMutex);
        simulatiosDataCv.wait(lock, [this, &nextSimulatorIdx]{ return !simulatorWasTaken[nextSimulatorIdx]; });
        if (nextSimulatorIdx >= numSimulators) {
            break;
        }
        int algoIdx = simulatorsCorrd[nextSimulatorIdx].first;
        int houseIdx = simulatorsCorrd[nextSimulatorIdx].second;
        simulatorWasTaken[nextSimulatorIdx] = true;
        nextSimulatorIdx++;
        
        threads.emplace_back([this, &runningThreadsMutex, &simulatiosCv, &dataMutex, &simulatiosDataCv, &scoreMutex, algoIdx, houseIdx]() {
            std::unique_ptr<MySimulator> simulator_ptr = std::move(simulators[algoIdx][houseIdx]);
            if (!simulator_ptr) {
                return;
            }
            simulators[algoIdx][houseIdx].reset();  // Prevent dangling reference
            simulatiosDataCv.notify_one();
            bool errorInRun = false;

            try {
                simulator_ptr->run();
            } catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
                errorInRun = true;
            }
            
            try {
                if (!summaryOnly) {
                    simulator_ptr->setOutput();
                }
            }
            catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }

            {
                std::lock_guard<std::mutex> lockRunning(runningThreadsMutex);
                runningThreads--;
            }

            {
                std::lock_guard<std::mutex> scoreLock(scoreMutex);
                if (!errorInRun) {
                    scores[algoIdx][houseIdx] = simulator_ptr->getScore();
                }
                else {
                    // TODO: set score as if run is faild!:MaxSteps * 2 + InitialDirt * 300 + 2000
                }
            }
            simulatiosCv.notify_one();
        });

    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            {
                std::lock_guard<std::mutex> lock(coutMutex);
                std::cerr << "join Thread [" << thread.get_id() << "]" << std::endl;
            }
            thread.join();
        }
    }
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
    // TODO: Add this!
    CsvManager csvManager(algorithmNames, housesNames, scores);
    csvManager.writeResultsToCsv();
}
