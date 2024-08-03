#include <string>
#include "../simulator/include/my_simulator.h"
#include "../simulator/include/my_algorithm.h"
#include "../simulator/include/logger.h"
#include "../simulator/include/common.h"

// getting command line arguments for the house file
int main(int argc, char** argv) {

    try {
        Common::checkForError(argc != 2, "Expecting house input file name, got " + std::to_string(argc) + " parameters.");
        MySimulator simulator;
        std::string houseFilePath = argv[1];
        simulator.readHouseFile(houseFilePath);
        MyAlgorithm algo;
        simulator.setAlgorithm(algo);
        simulator.run();
    }
    catch (const std::exception& e) {
        Logger::getInstance().getLogger()->error("{}", e.what(), ".");
        std::cerr << "Error: " << e.what() << std::endl;
        std::cout << "Error: " << e.what() << std::endl;
        std::cout.flush();
        return 1;
    }
    return 0;
}

