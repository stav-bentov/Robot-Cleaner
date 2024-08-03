#include "common/AlgorithmRegistrar.h"
#include "enums/common_enums.h"
#include "include/my_simulator.h"
#include <filesystem>
//#include <dlfcn>

void loadFiles(std::string& path, std::vector<std::string>& container, std::string extension) {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().extension() == extension) {
            container.push_back(entry.path().string());
        }
    }
}

void loadHouseFiles(std::string& housePath, std::vector<std::string>& houses){
    loadFiles(housePath, houses, ".house");
}

void loadAlgorithmFiles(std::string& algoPath, std::vector<std::string>& algorithms){
    loadFiles(algoPath, algorithms, ".so");
}

void openAlgorithms(std::vector<std::string>& algorithms, std::vector<void*> algorithmsHandle) {
    for (std::string algo : algorithms) {
        // TODO: Check if neccesecry to use c_str
        void* handle;
        /*handle = dlopen(algo.c_str(), RTLD_LAZY);
        // TODO: Write ERROR to the cureent working directory!!
        checkForError(!handle, "ERROR: Unable to open error file: " + algo);*/
        algorithmsHandle.push_back(handle);
    }
}

void readParameters(int argc, char* argv[], std::string& housePath, std::string& algoPath) {
    for (int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        // Check if argument starts with this sign
        if (arg.rfind("-house_path=", 0) == 0) {
            // "you are not required to have support for spaces around the equal sign"
            housePath = arg.substr(12);
        } else if (arg.rfind("-algo_path=", 0) == 0) {
            // "you are not required to have support for spaces around the equal sign"
            algoPath = arg.substr(11);
        }
    }
}

void runSimulations(std::vector<std::string>& houses) {
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
        if (algorithm) {
            std::cout << algo.name() << ": " << static_cast<int>(algorithm->nextStep()) << std::endl;
            for (const auto& house : houses) {
                std::cout << "Running simulation for House: " << house << " with Algorithm: " << algo.name() << std::endl;

                MySimulator simulator;
                simulator.setAlgorithm(*algorithm);
                simulator.run();
            }
        } 
        else {
            // TODO: handle error in algorithm
        }
    }
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
}

void closeAlgorithms(std::vector<void*> algorithmsHandle) {
    for (void* handle : algorithmsHandle) {
        //dlclose(handle);
    }
}

int main(int argc, char* argv[]) {
    // Default search will be in current directory
    std::string housePath = ".";
    std::string algoPath = ".";

    std::vector<std::string> houses;
    std::vector<std::string> algorithms;

    std::vector<void*> algorithmsHandle;

    readParameters(argc, argv, housePath, algoPath);
    loadHouseFiles(housePath, houses);
    loadAlgorithmFiles(algoPath, algorithms);
    openAlgorithms(algorithms, algorithmsHandle);
    runSimulations(houses);
    closeAlgorithms(algorithmsHandle);
    return 0;
}