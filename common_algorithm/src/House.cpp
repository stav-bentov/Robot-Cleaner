#include "../include/house.h"
#include <thread>
House::House(const std::string& filePath) {
    houseFilePath = filePath;
    createErrorName();
    try {
        getParameters(filePath);
    }
    catch (const std::exception& e) {
        ErrorManager::checkForError(true, e.what(), errorFileName);
        ErrorManager::checkForError(true, "Error: in house constructor");
    }
}

/*
    Get all house parameters using Input manager
*/
void House::getParameters(const std::string& filePath) {
    InputManager inputManager(filePath);
    inputManager.getParameters(houseName, maxSteps, maxBattery, amountOfDirt, rows, cols, dockingStationLocation, houseSurface);
    currentBatterySteps = maxBattery;
    currentLocation = dockingStationLocation;
}

void House::createErrorName() {
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
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";
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
}

void House::clean() {
    amountOfDirt--;
    houseSurface[currentLocation.first][currentLocation.second]--;
}

/*
	Get current step and update robot location in simulator and sensors
*/
void House::updateLocation(Step step) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";
    std::pair<int, int> stepElements = Common::stepMap.at(step);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
}

bool House::inDockingStation() const {
    return currentLocation == dockingStationLocation;
}

/*
	Make step- clean/ charge/ move and update robot location.
*/
void House::makeStep(Step step) {
    Direction d;
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

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
            d = Common::stepToDirection(step);
            ErrorManager::checkForError(isWall(d), thread +" Error: step is thoward the wall!");
            
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

std::string House::getHouseFilePath() const{
    return houseFilePath;
}