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
			std::cout <<"Simulator got Step::Stay" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::Stay");
			break;
		case Step::East:
			Logger::getInstance().log("Simulator got Step::East", LogLevels::FILE);
			std::cout <<"Simulator got Step::East" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::East");
			break;
		case Step::West:
			Logger::getInstance().log("Simulator got Step::West", LogLevels::FILE);
			std::cout <<"Simulator got Step::West" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::West");
			break;
		case Step::South:
			Logger::getInstance().log("Simulator got Step::South", LogLevels::FILE);
			std::cout <<"Simulator got Step::South" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::South");
			break;
		case Step::North:
			Logger::getInstance().log("Simulator got Step::North", LogLevels::FILE);
			std::cout <<"Simulator got Step::North" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::North");
			break;
		case Step::Finish:
			Logger::getInstance().log("Simulator got Step::Finish", LogLevels::FILE);
			std::cout <<"Simulator got Step::Finish" << std::endl;
			//Logger::getInstance().getLogger()->info("Simulator got Step::Finish");
			break;
	}
}


Direction Common::stepToDirection(Step s) {
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
        case Step::Stay:
        case Step::Finish:
		{
			// Assumption: stepToDirection is called only by direction<->step
			ErrorManager::checkForError(true, "Error: Got invalid step in stepToDirection");
			break;
		}
        default:
            throw std::runtime_error("Unknown step direction");
    }
    return d;
}