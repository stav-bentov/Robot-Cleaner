#include "../include/common_enums.h"
#include <thread>
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
	std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";
	switch (s)
	{
		case Step::Stay:
			Logger::getInstance().log(thread +" Simulator got Step::Stay", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::Stay in thread" << std::endl;
			break;
		case Step::East:
			Logger::getInstance().log(thread +" Simulator got Step::East", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::East in thread" << std::endl;
			break;
		case Step::West:
			Logger::getInstance().log(thread +" Simulator got Step::West", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::West in thread" << std::endl;
			break;
		case Step::South:
			Logger::getInstance().log(thread +" Simulator got Step::South", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::South in thread" << std::endl;
			break;
		case Step::North:
			Logger::getInstance().log(thread +" Simulator got Step::North", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::North in thread" << std::endl;
			break;
		case Step::Finish:
			Logger::getInstance().log(thread +" Simulator got Step::Finish", LogLevels::FILE);
			std::cout <<thread <<" Simulator got Step::Finish in thread" << std::endl;
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
			// Should NOT happen
            throw std::runtime_error("Unknown step direction");
    }
    return d;
}