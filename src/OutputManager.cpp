#include "../simulator/include/output_manager.h"

OutputManager::OutputManager(){}

void OutputManager::writeOutput(std::vector<Step> steps, std::size_t numSteps, int amountOfDirtLeft, std::string status) {
    std::ofstream file(outputName);
    Common::checkForError(!file.is_open(), "Cannot open output file: "+ outputName + "." );

    file << "NumSteps = " << numSteps << std::endl;
    file << "DirtLeft = " << amountOfDirtLeft << std::endl;
    file << "Status = " << status << std::endl;
    file << "Steps: " << std::endl;
    for (Step s : steps) {
        file << getChar(s);
    }
    file.close();
    Logger::getInstance().getLogger()->info("Done: Output is written to: {}.", outputName);
    std::cout << "Done: Output is written to: " << outputName << std::endl;
}

void OutputManager::setInputName(std::string& inputFileName) {
    inputName = inputFileName;
    createOuputName();
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

/*
    Extract name of input file from the path and add output_ to create output file name
*/
void OutputManager::createOuputName() {
    std::string fullPath = inputName;
    std::filesystem::path filePath(fullPath);
    std::string fileName = filePath.stem().string(); 
    std::string fileExtension = filePath.extension().string(); 
    std::string newFileName = "output_" + fileName + fileExtension;
    std::filesystem::path newFilePath = filePath.parent_path() / newFileName;
    outputName = newFilePath.string();
}


void OutputManager::displaySim() {
    if (configM.getDisplaySim()) {
        Logger::getInstance().getLogger()->info("Display simulation accepted according to config.json.");
        std::cout << "Display simulation accepted according to config.json." << std::endl;

        VisualSimulation simulation;
        simulation.changeInfoRepMapping(configM);
        simulation.changeColorMapping(configM);
        simulation.processFiles(inputName, outputName);
        simulation.startSimulation();
    }
    else
    {
        Logger::getInstance().getLogger()->info("Display simulation denied according to config.json.");
        std::cout << "Display simulation denied according to config.json." << std::endl;
    }
}