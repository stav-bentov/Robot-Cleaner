#include "../include/common_enums.h"


const std::map<Direction, std::pair<int, int>> Common::directionMap = {
        {Direction::East, {0, 1}},
        {Direction::West, {0, -1}},
        {Direction::South, {1, 0}},
        {Direction::North, {-1, 0}}
};

void Common::logStep(Step s) {
	switch (s)
	{
		case Step::Stay:
			Logger::getInstance().log("Simulator got Step::Stay", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::Stay");
			break;
		case Step::East:
			Logger::getInstance().log("Simulator got Step::East", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::East");
			break;
		case Step::West:
			Logger::getInstance().log("Simulator got Step::West", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::West");
			break;
		case Step::South:
			Logger::getInstance().log("Simulator got Step::South", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::South");
			break;
		case Step::North:
			Logger::getInstance().log("Simulator got Step::North", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::North");
			break;
		case Step::Finish:
			Logger::getInstance().log("Simulator got Step::Finish", 3);
			//Logger::getInstance().getLogger()->info("Simulator got Step::Finish");
			break;
	}
}

void Common::checkForError(bool value, std::string errorDescription) {
    if (value) {
        //Logger::getInstance().getLogger()->error(errorDescription);
        throw std::runtime_error(errorDescription); 
    }
}
