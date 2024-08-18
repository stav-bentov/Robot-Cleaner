#include "include/MainManager.h"
#include <chrono>

void MainManager::run(int argc, char* argv[]){
    readParameters(argc, argv);
    loadHouseFiles();
    loadAlgorithmFiles();
    openAlgorithms();
    createSimulations();
    closeAlgorithms();
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

void MainManager::createSimulations() {
    std::mutex cerr_mut;
    //results.resize(housespath.size(), std::vector<int>(algorithms.size(), -1));
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        for (auto& housePath : housespath) {
            std::cout << "Adding simulation for House: " << housePath << " with Algorithm: " << algo.name() << std::endl;
            try {
                    std::unique_lock<std::mutex> lock(runningThreadsMutex);
                    Logger::getInstance().log("runningThreads = " + std::to_string(runningThreads) + " .\n", LogLevels::FILE);
                    Logger::getInstance().log("numThreads = " + std::to_string(numThreads) + " .\n", LogLevels::FILE);
                    simulatiosCv.wait(lock, [this]{return runningThreads < numThreads;});
                    runningThreads++;
                    threads.emplace_back([this, &cerr_mut, &housePath, &algo] {
                        std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
                        if (algorithm) {

                            {
                                std::lock_guard<std::mutex> lock(cerr_mut);
                                std::cerr << "Thread [" << std::this_thread::get_id() << "] running simulation" << std::endl;
                            }
                            
                            MySimulator simulator;
                            simulator.prepareSimulationEnvironment(housePath, algo.name());
                            simulator.setAlgorithm(*algorithm);
                            std::cout << "simulator.run();" << std::endl;
                            simulator.run();
                            std::cout << "simulator.setOutput();" << std::endl;
                            if (summaryOnly) {
                                simulator.setOutput();
                            }
                            {
                                std::lock_guard<std::mutex> lock(runningThreadsMutex);
                                runningThreads--;
                            }
                            simulatiosCv.notify_one();
                        } else {
                            ErrorManager::checkForError(true, "Error: Failed to create algorithm.", algo.name() + ".error");
                        } 
                    });

            }
            catch (const std::exception& e) {
                std::cout << "Error: " << e.what() << std::endl;
            }
        }
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