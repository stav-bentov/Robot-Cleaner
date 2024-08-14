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
            numThread = std::stoi(arg.substr(11));
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
void MainManager::openAlgorithms(std::vector<std::string>& algorithms, std::vector<void*>& algorithmsHandle) {
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

void MainManager::runSimulations(std::vector<std::string>& houses) {
    /*CsvManager csvManager;
    int row = 0;
    int col = 0;
    int score = -1;*/

    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        //first = true;
        for (auto& house : houses) {
            //csvManager.addHouseName(house);
            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            if (algorithm) {

                /*if (first) {
                    first = false;
                    csvManager.addAlgorithmName(algo.name());
                }*/

                std::cout << algo.name() << ": " << static_cast<int>(algorithm->nextStep()) << std::endl;
                    std::cout << "Running simulation for House: " << house << " with Algorithm: " << algo.name() << std::endl;
                    try {
                        std::cout << "MySimulator simulator" << std::endl;
                        MySimulator simulator;
                        std::cout << "simulator.prepareSimulationEnvironment(house, algo.name())" << std::endl;
                        simulator.prepareSimulationEnvironment(house, algo.name());
                        std::cout << "simulator.setAlgorithm(*algorithm)" << std::endl;
                        simulator.setAlgorithm(*algorithm);
                        std::cout << "simulator.run()" << std::endl;
                        simulator.run();
                        std::cout << "simulator.setOutput();" << std::endl;
                        simulator.setOutput();
                    }
                    catch (const std::exception& e) {
                        std::cout << "Error: " << e.what() << std::endl;
                    }
                    
                    // Update csv
                    //csvManager.addScore(score, row, col);
                }
            else {
                ErrorManager::checkForError(true, "Error: Failed to create algorithm.", algo.name() + ".error");
            }
            //col+++;
        }
        //row++;
    }
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
}

void MainManager::closeAlgorithms(std::vector<void*> algorithmsHandle) {
    for (void* handle : algorithmsHandle) {
        dlclose(handle);
    }
}