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
	// TODO: maybe add handle error?
    SimConfigurationManager simConfigManager;
    milisecondPerStep = simConfigManager.getTimePerStep();
    std::cout <<" milisecond Per Step: " << milisecondPerStep << " milliseconds" << std::endl;
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
        }
        catch (const std::exception& e) {
            std::cout << "ERROR: Failed creating house: " << housePath << " Exception: " << e.what() << std::endl;
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
            std::cout << "Error loading algorithm library: No Algorithm Registered!" << std::endl;
        } else {
            algorithmsHandle.push_back(handle);
        }
    }
}

void MainManager::runTasks(boost::asio::io_context& ioContext) {
    int algoIdx = 0;
    int houseIdx = 0;
    bool validAlgo = true;
    
    std::mutex runningThreadsMutex;
    std::condition_variable simulatiosCv;

    const long numberOfSimulations = algorithmsPath.size()*houses.size();
    std::latch workDone(numberOfSimulations);

    scores.resize(algorithmsPath.size(), std::vector<int>(houses.size(), noResult));

    // Initiate tasks
    std::list<Task> tasks;
    for (const auto& algo : AlgorithmRegistrar::getAlgorithmRegistrar()) {
        algorithmNames.push_back(algo.name());
        for (std::shared_ptr<House> house: houses) {
            housesNames.push_back(house->getHouseName());
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
            tasks.emplace_back(std::move(algorithm), house, algoIdx, houseIdx, algo.name(), ioContext, workDone,
            summaryOnly, milisecondPerStep, runningThreads, runningThreadsMutex, simulatiosCv);
            houseIdx++;
        }
        algoIdx++;
        validAlgo = true;
        houseIdx = 0;
    }

    for (auto& task : tasks) {
        std::unique_lock<std::mutex> lockRunning(runningThreadsMutex);
        simulatiosCv.wait(lockRunning, [this]{ return runningThreads < numThreads; });
        runningThreads++;
        task.run();
    }
    // wait on latch, for all threads to report done
    workDone.wait();

    std::cout << "Results:" << std::endl;
    size_t taskIndex = 0;
    for (auto& task: tasks) {
        std::cout << " >>> Task " << taskIndex << " details: "<< task.getHouseName() << ", " << task.getAlgoName() << "score: " << task.getScore() << std::endl;
        scores[task.getAlgoIdx()][task.getHouseIdx()] = task.getScore();
        ++taskIndex;
        task.detach();
    }
}

void MainManager::manageTasks() {
    boost::asio::io_context ioContext;
    
    // Prevent the I/O context from stopping until all work is done.
    // Even if there is no active timer to wait for
    auto workGuard = boost::asio::make_work_guard(ioContext);
    
    // Run io_context in a separate thread
    std::jthread ioThread([&ioContext]() {
        ioContext.run();
    });

    runTasks(ioContext);
    // stop the io_context
    ioContext.stop();
    std::cout << "Done prepereForTasks" << std::endl;
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
