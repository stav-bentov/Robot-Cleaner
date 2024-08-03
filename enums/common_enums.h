#ifndef ENUMS_H_
#define ENUMS_H_

#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <functional>
#include <memory>
#include "../simulator/include/enums.h"
//#include "logger.h"

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

#endif  // ENUMS_H_