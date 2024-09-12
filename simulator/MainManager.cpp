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
void MainManager::loadFiles(std::string path, std::vector<std::string>& container, std::string extension) {
    // I have intention to send ERROR!
    ErrorManager::checkForError(!std::filesystem::exists(path), "Directory does not exist: " + path);
    ErrorManager::checkForError(!std::filesystem::is_directory(path), "Path is not a directory: " + path);
    
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

/*
    Creates House objects from loaded house 
    files and stores them in a container, also populates the house names.
*/
void MainManager::createHouses() {
    for (const auto& housePath : housespath) {
        try {
            auto sharedHouse = std::make_shared<House>(housePath);
            houses.emplace_back(sharedHouse);
            housesNames.push_back(sharedHouse->getHouseName());
        }
        catch (const std::exception& e) {
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
        void* handle = dlopen(algo.c_str(), RTLD_LAZY);
        if (!handle) {
            std::filesystem::path algoPath(algo);
            ErrorManager::checkForError(true, "ERROR: Unable to open error file: " + algo, algoPath.stem().string() + ".error");
        } else {
            algorithmsHandle.push_back(handle);
        }
    }
}

/*
    Creates Task objects for each combination of algorithm and house, with corresponding properties
     and adds them to the task list.
*/
void MainManager::createTasks(std::list<Task>& tasks, boost::asio::io_context& ioContext, 
                              std::latch& workDone, std::counting_semaphore<>& semaphore) {
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
            tasks.emplace_back(std::move(algorithm), houseCopy, algoIdx, houseIdx, algo.name(), ioContext, 
                                workDone, summaryOnly, milisecondPerStep, semaphore);
            houseIdx++;
        }
        algoIdx++;
        validAlgo = true;
        houseIdx = 0;
    }
}

/*
    Executes tasks from the list by managing thread limits 
    and waiting for the condition variable to signal when a thread slot is available.
*/
void MainManager::runTasks(std::list<Task>& tasks, std::counting_semaphore<>& semaphore) {
    for (auto& task : tasks) {
        semaphore.acquire();  // Wait for an available thread
        task.run();
    }
}

/*
    Writes the output of each task to files 
    if not in summary-only mode, updates task scores, and detaches tasks.
*/
void MainManager::writeOutputFiles(std::list<Task>& tasks) {
    size_t taskIndex = 0;
    for (auto& task: tasks) {
        task.setOutputAndCalcScore(!summaryOnly);
        scores[task.getAlgoIdx()][task.getHouseIdx()] = task.getScore();
        ++taskIndex;
        task.detach();
    }
    
}

/*
    Manages the overall task execution process, including creating tasks, 
    running them, waiting for completion, and handling the I/O context.
*/
void MainManager::manageTasks() {
    boost::asio::io_context ioContext;
    const long numberOfSimulations = algorithmsHandle.size()*houses.size();
    std::latch workDone(numberOfSimulations);
    scores.resize(algorithmsHandle.size(), std::vector<int>(houses.size(), noResult));
    std::list<Task> tasks;
    std::counting_semaphore<> semaphore(numThreads);

    // Prevent the I/O context from stopping until all work is done.
    // Even if there is no active timer to wait for
    auto workGuard = boost::asio::make_work_guard(ioContext);
    
    // Run io_context in a separate thread
    std::jthread ioThread([&ioContext]() {
        ioContext.run();
    });

    createTasks(tasks, ioContext, workDone, semaphore);

    runTasks(tasks, semaphore);

    // Wait on latch, for all threads to report done
    workDone.wait();

    // stop the io_context
    ioContext.stop();
    
    writeOutputFiles(tasks);
}

/*
    Clears algorithm registrations and closes all opened algorithm handles.
*/
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
