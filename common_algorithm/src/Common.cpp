#include "../include/common_enums.h"

const std::map<Step, std::pair<int, int>> Common::stepMap = {
    {Step::North, {-1, 0}},
    {Step::East, {0, 1}},
    {Step::South, {1, 0}},
    {Step::West, {0, -1}},
    {Step::Stay, {0, 0}},
    {Step::Finish, {0, 0}}
};

const std::map<Direction, std::pair<int, int>> Common::directionMap = {
        {Direction::North, {-1, 0}},
        {Direction::East, {0, 1}},
        {Direction::South, {1, 0}},
        {Direction::West, {0, -1}}
};

void Common::logStep(Step s) {
	switch (s)
	{
		case Step::Stay:
			Logger::getInstance().log("Simulator got Step::Stay", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::Stay");
			break;
		case Step::East:
			Logger::getInstance().log("Simulator got Step::East", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::East");
			break;
		case Step::West:
			Logger::getInstance().log("Simulator got Step::West", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::West");
			break;
		case Step::South:
			Logger::getInstance().log("Simulator got Step::South", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::South");
			break;
		case Step::North:
			Logger::getInstance().log("Simulator got Step::North", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::North");
			break;
		case Step::Finish:
			Logger::getInstance().log("Simulator got Step::Finish", LogLevels::FILE);
			//Logger::getInstance().getLogger()->info("Simulator got Step::Finish");
			break;
	}
}


Direction stepToDirection(Step s) {
    Direction d;
    switch (s)
    {
        case Step::North:
            d = Direction::North;
            break;
        case Step::East:
            d = Direction::East;
            break;
        case Step::South:
            d = Direction::South;
            break;
        case Step::West:
            d = Direction::West;
            break;
    }
    return d;
}