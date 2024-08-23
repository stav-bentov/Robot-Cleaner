#include "../include/error_manager.h"

void ErrorManager::checkForError(bool declareError, std::string errorDescription, std::string errorFile) {
    if (declareError) {
        std::cerr << "Error: " << errorDescription << std::endl; 
        std::ofstream file(errorFile, std::ios::app);
        if (file.is_open()) {
            std::cerr << "Wrote to file:" << errorFile << std::endl;
            file << errorDescription << std::endl;
        }
        else {
            std::cerr << "Did not Wrote to file:" << errorFile << std::endl;
        }
        file.close();
    }
}


void ErrorManager::checkForError(bool declareError, std::string errorDescription) {
    if (declareError) {
        std::cerr << "Error: " << errorDescription << std::endl; 
        throw std::runtime_error(errorDescription); 
    }
}