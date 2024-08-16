#ifndef COMMON_H_
#define COMMON_H_

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <array>
#include <fstream> 
#include <iostream> 
#include <sstream>
#include "logger.h"
#include "../../common/enums.h"
#include "error_manager.h"

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
        //static void checkForError(bool value, std::string errorDescription);
        static void logStep(Step s);
        static const std::map<Direction, std::pair<int, int>> directionMap;
        static const std::map<Step, std::pair<int, int>> stepMap;
        static Direction stepToDirection(Step s) ;

};

#endif  // COMMON_H_