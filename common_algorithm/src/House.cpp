#include "../include/house.h"


const std::map<Step, std::pair<int, int>> House::stepMap = {
    {Step::North, {-1, 0}},
    {Step::East, {0, 1}},
    {Step::South, {1, 0}},
    {Step::West, {0, -1}},
    {Step::Stay, {0, 0}},
    {Step::Finish, {0, 0}}
};

House::House(std::string& filePath) {
    dockingStationLocation = {-1, -1};
    amountOfDirt = 0;

    // Open the file 
    std::ifstream file(filePath); 

    // Check if the file was opened successfully 
    Common::checkForError(!file.is_open(), "Error: Cannot open input file: " + filePath + ".");
    
    // Read parameters and house surface from file
    processParametersFromFile(file);
    processHouseMappingFromFile(file);
   
    file.close();
    Common::checkForError(dockingStationLocation == std::make_pair(-1,-1), "Error: No docking station."); 

    currentBatterySteps = maxBattery;
    currentLocation = dockingStationLocation;
}

void House::processParametersFromFile(std::ifstream& file) {
    std::string line;
    std::string temp;
    std::stringstream ss;

    // Read house name
    Common::checkForError(!std::getline(file, houseName), "Error: failed to read house name."); 
    
    // Read maximum steps
    processVariable(file, maxSteps, "Error: failed to read maximum steps.");

    // Read battery steps
    processVariable(file, maxBattery, "Error: failed to read maximum battery steps.");

    // Read number of rows
    processVariable(file, rows, "Error: failed to read number of rows.");

    // Read number of cols
    processVariable(file, cols, "Error: failed to read number of columns.");

    Common::checkForError(!(rows >=0 && cols >= 0), "Error: one of the parameters is less then 0- update it"); 

    
    /*Logger::getInstance().getLogger()->info("Proccessed parameters: maximum steps = " 
                                            + std::to_string(maxSteps) + ", battery steps = " + std::to_string(maxBattery) 
                                            + ", number of rows = " + std::to_string(rows) + ", number of cols = " 
                                            + std::to_string(cols) + ".");*/
}

template<typename T>
void House::processVariable(std::ifstream& file, T& var, const std::string& errorMessage) {
    std::string line;
    std::string str;
    int val;

    Common::checkForError(!(std::getline(file, line)), errorMessage); 
    
    auto equalSignPos = line.find('=');
    if (equalSignPos == std::string::npos) {
        Common::checkForError(true, "Error: Missing equal sign in line: " + line);
    }
    std::string value = line.substr(equalSignPos + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    std::cout << "Value: '" << value << "'" << std::endl;
    // Convert value to integer
    try {
        val = std::stoi(value);  // Use std::stoi for conversion
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Error: Invalid input value: " + value);
    } catch (const std::out_of_range&) {
        throw std::runtime_error("Error: Input value out of range: " + value);
    }

    // Check for negative values
    if (val < 0) {
        throw std::runtime_error("Error: Invalid input value (negative number): " + value);
    }

    // Assign to var
    var = static_cast<T>(val);

    // Debug print for the converted integer
    std::cout << "Converted value: " << static_cast<int>(var) << std::endl;
}

void House::processHouseMappingFromFile(std::ifstream& file) {
    std::string line;
    std::vector<int> row;
    char c;

    // Read layout
    for (int i = 0; i < rows; ++i) {
        row.clear();
        if (!std::getline(file, line)) {
            houseSurface.push_back(std::vector<int>(cols, 0));
            continue;
        }

        /*
            "If there are missing lines, they should be filled in with empty lines,
            space as well as any character which does not have other mapping below"= corridors
        */ 
        if (line.empty()) {
            houseSurface.push_back(std::vector<int>(cols, 0));
            continue;
        }

        for (int j = 0; j < cols; ++j) {
            if (j >= static_cast<int>(line.size())) {
                row.push_back(0);
            } else {
                c = line[j];
                
                if (c == 'D') { // Docking station
                    // Docking station is already set if dockingStationLocation != {-1, -1} 
                    Common::checkForError(dockingStationLocation != std::make_pair(-1, -1), "Invalid house format: there should be exactly one docking station");
                    dockingStationLocation = std::make_pair(i, j);
                    row.push_back(static_cast<int>(Elements::DockingStation));
                } else if (c == 'W') { // Wall
                    row.push_back(static_cast<int>(Elements::Wall));
                } else if (isdigit(c)) { // Floor with dirt
                    amountOfDirt += stoi(std::string(1, c));
                    row.push_back(stoi(std::string(1, c)));
                } else { //"Space as well as any character which does not have other mapping below"
                    row.push_back(0);
                }      
            }
        }
        houseSurface.push_back(row);
    }
    logHouseSurface();
}

void House::logHouseSurface() {
    std::stringstream ss;
    for (const auto& row : houseSurface) {
        for (const auto& cell : row) {
            ss << cell << " ";
        }
        ss << "\n";
    }
    //Logger::getInstance().getLogger()->info("House Layout:\n{}", ss.str());
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
    Logger::getInstance().log("House:: getDirtLevel in " + std::to_string(currentLocation.first) + ","+ std::to_string(currentLocation.second) + ":" + std::to_string(houseSurface[currentLocation.first][currentLocation.second]) + "\n", 3);

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
    Logger::getInstance().log("Discharged, current battary:" + std::to_string(currentBatterySteps) + ".\n", 3);
}

void House::clean() {
    std::cout << "Cleaned: " << currentLocation.first << ", " << currentLocation.second << std::endl;
    amountOfDirt--;
    houseSurface[currentLocation.first][currentLocation.second]--;
}

/*
	Get current step and update robot location in simulator and sensors
*/
void House::updateLocation(Step step) {
    Logger::getInstance().log("House:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", 3);

    std::pair<int, int> stepElements = stepMap.at(step);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;

    Logger::getInstance().log("House:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", 3);
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