#include "../include/common.h"


void Common::checkForError(bool value, std::string errorDescription) {
    if (value) {
        Logger::getInstance().getLogger()->error(errorDescription);
        throw std::runtime_error(errorDescription); 
    }
}
