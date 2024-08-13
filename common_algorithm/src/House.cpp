#include "../include/house.h"

House::House(std::string& filePath) {
    try {
        getParameters(filePath);
    }
    catch (const std::exception& e) {
        ErrorManager::checkForError(true, e, houseName);
        ErrorManager::checkForError(true, "Error: in house constructor");
    }
}

/*
    Get all house parameters using Input manager
*/
void House::getParameters(std::string& filePath) {
    InputManager im(filePath);
    im.getParameters(houseName, maxSteps, maxBattery, amountOfDirt, rows, cols, dockingStationLocation, houseSurface);
    createErrorName(filePath);
    currentBatterySteps = maxBattery;
    currentLocation = dockingStationLocation;
}

void House::createErrorName(std::string& houseFilePath) {
    std::filesystem::path housePath(houseFilePath);

    // Get the names
    std::string houseName = housePath.stem().string(); 

    errorFileName = houseName + ".error";
}

std::size_t House::getMaxSteps() const {
    return maxSteps;
}

std::size_t House::getMaxBattery() const {
    return maxBattery;
}

int House::getAmountOfDirt() const {
    return amountOfDirt;
}

std::pair<int, int> House::getDockingStationLocation() const {
    return dockingStationLocation;
}

// For simulation
std::vector<std::vector<int>> House::getHouseSurface() const {
    return houseSurface;
}

int House::getDirtLevel() const{
    Logger::getInstance().log("House:: getDirtLevel in " + std::to_string(currentLocation.first) + ","+ std::to_string(currentLocation.second) + ":" + std::to_string(houseSurface[currentLocation.first][currentLocation.second]) + "\n", LogLevels::FILE);

    return houseSurface[currentLocation.first][currentLocation.second];
}

bool House::isWall(std::pair<int, int> location) const{
    // Check if in boundries
    if (location.first < 0 || location.first >= rows || location.second < 0 || location.second >= cols) {
        return true; 
    }
    return houseSurface[location.first][location.second] == static_cast<int>(Elements::Wall);

}

float House::getCurentBatterySteps() const {
    return currentBatterySteps;
}

void House::charge() {
    double charge_increment = (maxBattery)/20.0;
    // Make sure the battery is not overloaded
    currentBatterySteps = std::min(currentBatterySteps + charge_increment, static_cast<double>(maxBattery));
}

void House::discharge() {
    currentBatterySteps--;
    Logger::getInstance().log("Discharged, current battary:" + std::to_string(currentBatterySteps) + ".\n", LogLevels::FILE);
}

void House::clean() {
    Logger::getInstance().log("Cleaned in location: " + std::to_string(currentLocation.first) + ", " +std::to_string(currentLocation.second) + ".\n", LogLevels::FILE);
    amountOfDirt--;
    houseSurface[currentLocation.first][currentLocation.second]--;
}

/*
	Get current step and update robot location in simulator and sensors
*/
void House::updateLocation(Step step) {
    Logger::getInstance().log("House:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", LogLevels::FILE);

    std::pair<int, int> stepElements = Common::stepMap.at(step);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;

    Logger::getInstance().log("House:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", LogLevels::FILE);
}

bool House::inDockingStation() const {
    return currentLocation == dockingStationLocation;
}

/*
	Make step- clean/ charge/ move and update robot location.
*/
void House::makeStep(Step step) {
    switch (step)
	{
		case Step::Stay:
			/* We are stying in place in 2 situations:
				- In docking station for charging
				- In dirty floor for cleaning
			*/
			if (inDockingStation()) {
				// Charge
				charge();
			}
			else {
				// Clean
				clean();
				discharge();
			}
			break;
		case Step::West:
		case Step::East:
		case Step::North:
		case Step::South:
            // Check valid step
            Direction d = Common::stepToDirection(step);
            ErrorManager::checkForError(isWall(d), "Error: step is thoward the wall!");
            
			discharge();
			break;
		default:
			break;
	}

	// Update robot location if needed
    // Every Step considered as step (in cleaning, moving and charging)
    updateLocation(step);
}

bool House::isWall(Direction d) const {
    std::pair<int, int> dirElements = Common::directionMap.at(d);
    int row = currentLocation.first + dirElements.first;
    int col = currentLocation.second + dirElements.second;
    return isWall({row, col});
}

std::string House::getHouseName() const{
    return houseName;
}