#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]){
    readParameters(argc, argv);
    getMaxSteps();
    loadHouseFiles();
    loadAlgorithmFiles();
    openAlgorithms();
    createSimulations();
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

void MainManager::getMaxSteps() {
    SimConfigurationManager simConfigManager;
    maxSteps = simConfigManager.getMaxSteps();
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

void MainManager::createSimulations() {
    std::mutex cerr_mut;

    int algo_idx = 0;
    int house_idx = 0;
    scores.resize(housespath.size(), std::vector<int>(algorithms.size(), noResult));

    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        for (auto& housePath : housespath) {
            housesNames.push_back(housePath.stem().string());
            std::cout << "Adding simulation for House: " << housePath << " with Algorithm: " << algo.name() << std::endl;
            try {
                std::unique_lock<std::mutex> lock(runningThreadsMutex);

                Logger::getInstance().log("runningThreads = " + std::to_string(runningThreads) + " .\n", LogLevels::FILE);
                Logger::getInstance().log("numThreads = " + std::to_string(numThreads) + " .\n", LogLevels::FILE);

                simulatiosCv.wait(lock, [this]{return runningThreads < numThreads;});

                runningThreads++;
                std::atomic<bool> stopFlag(false); // Stop flag for this thread

                // TODO: understand if & is neccesery
                threads.emplace_back([this, &cerr_mut, &housePath, &algo, &stopFlag, &algo_idx, &house_idx] {
                    std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
                    if (algorithm) {

                        {
                            std::lock_guard<std::mutex> lock(cerr_mut);
                            std::cout << "Thread [" << std::this_thread::get_id() << "] running simulation" << std::endl;
                        }
                        MySimulator simulator(stopFlag);
                        simulator.prepareSimulationEnvironment(housePath, algo.name());
                        simulator.setAlgorithm(*algorithm);
                        
                        // Set up a timer for this thread
                        boost::asio::io_context ioContext;
                        boost::asio::steady_timer timer(ioContext, std::chrono::seconds(1)); 
                        timer.async_wait([&](const boost::system::error_code& ec) { 
                            if (!ec) { // No error means the timeout occurred
                                {
                                    std::lock_guard<std::mutex> lock(cerr_mut);
                                    std::cout << " " << std::endl;
                                    std::cout << "Timeout occurred for thread [" << std::this_thread::get_id() << "]" << std::endl;
                                    std::cout << " " << std::endl;
                                }
                                stopFlag.store(true);
                            }
                            else {
                                std::lock_guard<std::mutex> lock(cerr_mut);
                                std::cout << " " << std::endl;
                                std::cout << "error with timout for thread [" << std::this_thread::get_id() << "]" << std::endl;
                                std::cout << " " << std::endl;
                            }
                        }); 
                        
                        ioContext.run(); 
                        simulator.run();

                        timer.cancel();

                        if (summaryOnly) {
                            simulator.setOutput();
                        }

                        scores[algo_idx][house_idx] = simulator.getScore();

                        {
                            std::lock_guard<std::mutex> lock(runningThreadsMutex);
                            runningThreads--;
                        }

                        simulatiosCv.notify_one();
                    } else {
                        ErrorManager::checkForError(true, "Error: Failed to create algorithm.", algo.name() + ".error");
                    } 
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
                std::lock_guard<std::mutex> lock(cerr_mut);
                std::cerr << "join Thread [" << std::this_thread::get_id() << "]" << std::endl;
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
    CsvManager csvManager(algorithmNames, housesNames, scores);
    csvManager.writeResultsToCsv();
}