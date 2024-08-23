#include "../include/input_manager.h"

InputManager::InputManager(const std::string& housePath) {
    dockingStationLocation = {-1, -1};
    amountOfDirt = 0;

    // Open the file 
    std::ifstream file(housePath); 
    // Check if the file was opened successfully 
    ErrorManager::checkForError(!file.is_open(), "Error: Cannot open input file: " + housePath + ".");
    // Read parameters and house surface from file
    processParametersFromFile(file);
    processHouseMappingFromFile(file);
    file.close();

    // Check if docking station exist
    ErrorManager::checkForError(dockingStationLocation == std::make_pair(-1,-1), "Error: No docking station."); 
}

void InputManager::processParametersFromFile(std::ifstream& file) {
    std::string line;
    std::string temp;
    std::stringstream ss;

    // Read house name
    ErrorManager::checkForError(!std::getline(file, houseName), "Error: failed to read house name."); 
    
    // Read maximum steps
    processVariable(file, maxSteps, "Error: failed to read maximum steps.");

    // Read battery steps
    processVariable(file, maxBattery, "Error: failed to read maximum battery steps.");

    // Read number of rows
    processVariable(file, rows, "Error: failed to read number of rows.");

    // Read number of cols
    processVariable(file, cols, "Error: failed to read number of columns.");

    ErrorManager::checkForError(!(rows >=0 && cols >= 0), "Error: one of the parameters is less then 0- update it"); 

    
  //  Logger::getInstance().log("Proccessed parameters: maximum steps = " + std::to_string(maxSteps) + ", battery steps = " + std::to_string(maxBattery) + ", number of rows = " + std::to_string(rows) + ", number of cols = " + std::to_string(cols) + ".", LogLevels::FILE);
}

/*
    Set parmeters (from input manager to house)
*/
void InputManager::getParameters(std::string& _houseName, std::size_t& _maxSteps, std::size_t& _maxBattery, int& _amountOfDirt,
                int& _rows, int& _cols, std::pair<int, int>& _dockingStationLocation, std::vector<std::vector<int>>& _houseSurface) const {
    _houseName = houseName;
    _maxSteps = maxSteps;
    _maxBattery = maxBattery;
    _amountOfDirt = amountOfDirt;
    _rows = rows;
    _cols = cols;
    _dockingStationLocation = dockingStationLocation;
    _houseSurface = houseSurface;
}

template<typename T>
void InputManager::processVariable(std::ifstream& file, T& var, const std::string& errorMessage) {
    std::string line;
    std::string str;
    int val;

    ErrorManager::checkForError(!(std::getline(file, line)), errorMessage); 
    
    auto equalSignPos = line.find('=');
    if (equalSignPos == std::string::npos) {
        ErrorManager::checkForError(true, "Error: Missing equal sign in line: " + line);
    }

    std::string value = line.substr(equalSignPos + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);

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
}

void InputManager::processHouseMappingFromFile(std::ifstream& file) {
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
                    ErrorManager::checkForError(dockingStationLocation != std::make_pair(-1, -1), "Invalid house format: there should be exactly one docking station");
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
}