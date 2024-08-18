#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]){
    readParameters(argc, argv);
    loadHouseFiles();
    loadAlgorithmFiles();
    openAlgorithms();
    createSimulations();
    runSimulations();
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
                numThread = std::stoi(arg.substr(13));
                if (numThread <= 0) {
                    throw std::runtime_error("Invalid number of threads provided. Using default: 10.");
                }
            } catch (const std::exception& e) {
                std::cerr << "Invalid number of threads provided. Using default: 10." << std::endl;
                numThread = 10;
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
        void* handle = dlopen(algo.c_str(), RTLD_LAZY);
        if (!handle)
        {
            std::filesystem::path algoPath(algo);
            ErrorManager::checkForError(true, "ERROR: Unable to open error file: " + algo, algoPath.stem().string() + ".error");
        }
        else
        {
            algorithmsHandle.push_back(handle);
        }
    }
}

void MainManager::createSimulations() {
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        for (auto& housePath : housespath) {
            std::cout << "Adding simulation for House: " << housePath << " with Algorithm: " << algo.name() << std::endl;
            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            if (algorithm) {
                try {
                    auto simulator = std::make_unique<MySimulator>();
                    simulator->prepareSimulationEnvironment(housePath, algo.name());
                    simulator->setAlgorithm(*algorithm);
                    simulations.push_back(std::move(simulator));
                }
                catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << std::endl;
                    simulations.push_back(nullptr);
                }
            }
            else
            {
                ErrorManager::checkForError(true, "Error: Failed to create algorithm.", algo.name() + ".error");
            }
        }
    }
}

void MainManager::runSimulations() {
    for(const auto& simulationPtr: simulations) {
        if (simulationPtr != nullptr) {
            threadSim(*simulationPtr);
        }
    }
}

void MainManager::threadSim(MySimulator& simulator) {
    try {
        std::cout << "simulator.run();" << std::endl;
        simulator.run();
        std::cout << "simulator.setOutput();" << std::endl;
        simulator.setOutput();
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}

void MainManager::closeAlgorithms() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    for (void* handle : algorithmsHandle) {
        dlclose(handle);
    }
}