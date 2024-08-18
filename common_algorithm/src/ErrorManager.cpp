#include "../include/error_manager.h"

void ErrorManager::checkForError(bool declareError, std::string errorDescription, std::string errorFile) {
    if (declareError) {
        std::ofstream logFile(errorFile, std::ios_base::app);
        if (logFile.is_open()) {
            logFile << errorDescription << std::endl;
        }
        logFile.close();
    }
}


void ErrorManager::checkForError(bool declareError, std::string errorDescription) {
    if (declareError) {
        throw std::runtime_error(errorDescription); 
    }
}