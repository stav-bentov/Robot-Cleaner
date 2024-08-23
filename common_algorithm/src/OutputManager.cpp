#include "../include/output_manager.h"

OutputManager::OutputManager(std::string housePath, std::string algoName) {
    houseFilePath = housePath;
    algorithmName = algoName;
    createOuputName();
}

/*
    Extract name of input file from the path and add output_ to create output file name
*/
void OutputManager::createOuputName() {
    std::filesystem::path housePath(houseFilePath);

    // Get the names
    std::string houseName = housePath.stem().string(); 

    outputName = houseName + "-" + algorithmName + ".txt";
}
void OutputManager::writeOutput(std::vector<Step> steps, std::size_t numSteps, int amountOfDirtLeft, std::string status, bool inDocking, int score) {
    std::ofstream file(outputName);
    ErrorManager::checkForError(!file.is_open(), "Cannot open output file: "+ outputName + "." );
    file << "NumSteps = " << numSteps << std::endl;
    file << "DirtLeft = " << amountOfDirtLeft << std::endl;
    file << "Status = " << status << std::endl;
    file << "InDock = " << (inDocking ? "TRUE" : "FALSE") << std::endl;
    file << "Score = " << score << std::endl;
    file << "Steps: " << std::endl;
    for (Step s : steps) {
        file << getChar(s);
    }
    file.close();
    std::cerr << "Done: Output is written to: " << outputName << std::endl;
}

std::string OutputManager::getChar(Step s) {
    std::string str;
    switch (s)
    {
        case Step::North:
            str = "N";
            break;
        case Step::East:
            str = "E";
            break;
        case Step::South:
            str = "S";
            break;
        case Step::West:
            str = "W";
            break;
        case Step::Stay:
            str = "s";
            break;
        case Step::Finish:
            str = "F";
            break;
    }
    return str;
}



void OutputManager::displaySim() {
    if (configM.getDisplaySim()) {
        VisualSimulation simulation;
        simulation.changeInfoRepMapping(configM);
        simulation.changeColorMapping(configM);
        simulation.processFiles(houseFilePath, outputName);
        simulation.startSimulation();
    }
}