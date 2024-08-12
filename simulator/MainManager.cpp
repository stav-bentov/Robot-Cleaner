#include "include/MainManager.h"

void MainManager::run(int argc, char* argv[]){
    // Default search will be in current directory


    std::vector<std::string> houses;
    std::vector<std::string> algorithms;

    std::vector<void*> algorithmsHandle;

    readParameters(argc, argv, housePath, algoPath);
    loadHouseFiles(housePath, houses);
    loadAlgorithmFiles(algoPath, algorithms);
    openAlgorithms(algorithms, algorithmsHandle);
    runSimulations(houses);
    closeAlgorithms(algorithmsHandle);
}


/*
    Reads parameters given in command line
*/
void MainManager::readParameters(int argc, char* argv[], std::string& housePath, std::string& algoPath) {
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
            // TODO: Add error handle
            numThread = std::to_integer(arg.substr(11));
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
void MainManager::loadHouseFiles(std::string& housePath, std::vector<std::string>& houses){
    loadFiles(housePath, houses, ".house");
}

/*
    Load algorithms (.so) files from housePath
*/
void MainManager::loadAlgorithmFiles(std::string& algoPath, std::vector<std::string>& algorithms){
    loadFiles(algoPath, algorithms, ".so");
}

/*
    For each .so algorithm (gets its path and name)- opens it using dlopen
*/
void MainManager::openAlgorithms(std::vector<std::string>& algorithms, std::vector<void*> algorithmsHandle) {
    for (std::string algo : algorithms) {
        void* handle;
        std::cout << "Trying to open: " << algo.c_str() << std::endl;
        handle = dlopen(algo.c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error: " << dlerror() << std::endl;
            std::cerr << "System error: " << strerror(errno) << std::endl;
        }
        // TODO: Write ERROR to the cureent working directory!!
        Common::checkForError(!handle, "ERROR: Unable to open error file: " + algo);
        algorithmsHandle.push_back(handle);
    }
}

void MainManager::runSimulations(std::vector<std::string>& houses) {
    std::cout << "in run simulation" << std::endl;
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        std::cout << "algo.name(): " << algo.name() << std::endl;
        std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
        if (algorithm) {
            std::cout << algo.name() << ": " << static_cast<int>(algorithm->nextStep()) << std::endl;
            for (auto& house : houses) {
                std::cout << "Running simulation for House: " << house << " with Algorithm: " << algo.name() << std::endl;

                MySimulator simulator;
                simulator.prepareSimulationEnvironment(house, algo.name());
                simulator.setAlgorithm(*algorithm);
                simulator.run();
                simulator.setOutput();
            }
        } 
        else {
            // TODO: handle error in algorithm
        }
    }
    std::cout << "done run simulation" << std::endl;

    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    std::cout << "AlgorithmRegistrar::getAlgorithmRegistrar().clear()" << std::endl;
}

void MainManager::closeAlgorithms(std::vector<void*> algorithmsHandle) {
    std::cout << "in run closeAlgorithms" << std::endl;
    for (void* handle : algorithmsHandle) {
        dlclose(handle);
    }
    std::cout << "done run closeAlgorithms" << std::endl;
}