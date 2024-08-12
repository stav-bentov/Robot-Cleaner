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

void OutputManager::writeOutput(std::vector<Step> steps, std::size_t numSteps, int amountOfDirtLeft, std::string status, bool inDocking, std::size_t maxSteps) {
    std::ofstream file(outputName);
    Common::checkForError(!file.is_open(), "Cannot open output file: "+ outputName + "." );

    file << "NumSteps = " << numSteps << std::endl;
    file << "DirtLeft = " << amountOfDirtLeft << std::endl;
    file << "Status = " << status << std::endl;
    file << "InDock = " << inDocking << std::endl;
    file << "Score = " << getScore(numSteps, status, amountOfDirtLeft,inDocking, maxSteps) << std::endl;
    file << "Steps: " << std::endl;
    for (Step s : steps) {
        file << getChar(s);
    }
    file.close();
    //Logger::getInstance().getLogger()->info("Done: Output is written to: {}.", outputName);
    std::cout << "Done: Output is written to: " << outputName << std::endl;
}

int OutputManager::getScore(std::size_t numSteps, std::string status, int amountOfDirtLeft, bool inDocking, std::size_t maxSteps) {
    if (status == "DEAD")
        return maxSteps + amountOfDirtLeft * 300 + 2000;
    else if (status == "FINISHED" && !inDocking)
        return maxSteps + amountOfDirtLeft * 300 + 3000;
    // else
    return numSteps + amountOfDirtLeft * 300 + (inDocking ? 0 : 1000);
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
        //Logger::getInstance().getLogger()->info("Display simulation accepted according to config.json.");
        std::cout << "Display simulation accepted according to config.json." << std::endl;

        VisualSimulation simulation;
        simulation.changeInfoRepMapping(configM);
        simulation.changeColorMapping(configM);
        simulation.processFiles(houseFilePath, outputName);
        simulation.startSimulation();
    }
    else
    {
        //Logger::getInstance().getLogger()->info("Display simulation denied according to config.json.");
        std::cout << "Display simulation denied according to config.json." << std::endl;
    }
}