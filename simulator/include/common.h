#ifndef COMMON_H_
#define COMMON_H_

#include <string>
#include <stdexcept>
#include "logger.h"

enum class Elements {
    Wall = -1,
    DockingStation = -2,
    Robot = -3,
    Direction = -4,
    EmptyWall = -5
};

enum class Type {DockingStation, Wall, Floor};

class Common {
    public:
        static void checkForError(bool value, std::string errorDescription);
};

#endif  // COMMON_H_