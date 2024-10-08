#ifndef ERROR_MANAGER_H_
#define ERROR_MANAGER_H_

#include <memory>
#include <cstddef>
#include <algorithm>  
#include <stdexcept>
#include <type_traits>
#include "common_enums.h"

class ErrorManager {
    public:
        static void checkForError(bool declareError, std::string errorDescription, std::string errorFile);
        static void checkForError(bool declareError, std::string errorDescription);
};
#endif  // ERROR_MANAGER_H_