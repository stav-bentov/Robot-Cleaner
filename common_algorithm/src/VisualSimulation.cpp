#include "../include/visual_simulation.h"

void VisualSimulation::processFiles(std::string inputFileName, std::string outputFileName) {
    processInputFile(inputFileName);
    processOutputFile(outputFileName);
}

/*
    Process input file by using House class and builds the map for the visual simulation
*/
void VisualSimulation::processInputFile(std::string& fileName) {
    House h(fileName);
    loadParameters(h);
    loadHouseMapping(h);
}

/* Process output file that has a known structure*/
void VisualSimulation::processOutputFile(std::string& fileName) {
    std::ifstream file(fileName);
    std::string line;
    std::string str;

    std::istringstream iss;  // Single instance of std::istringstream

    // Reading Num Steps
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Num Steps"); 
    iss.clear();  // Clear any error flags
    iss.str(line);  // Set new string buffer
    std::getline(iss, str, '=');
    iss >> numSteps;

    // Reading Dirt Left
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Dirt Left"); 
    iss.clear();
    iss.str(line);
    std::getline(iss, str, '=');
    iss >> dirtLeft;

    // Reading Status
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Status"); 
    iss.clear();
    iss.str(line);
    std::getline(iss, str, '=');
    iss >> status;

    // Reading InDock
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read InDock"); 
    iss.clear();
    iss.str(line);
    std::getline(iss, str, '=');
    iss >> status;
    
    // Reading Score
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Score"); 
    iss.clear();
    iss.str(line);
    std::getline(iss, str, '=');
    iss >> status;

    // Reading the Steps line
    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Steps");
    if (numSteps == 0) {
        return;
    }

    ErrorManager::checkForError(!(std::getline(file, line)), "Error: failed to read Steps");

    for (char c : line) {
        try {
            steps.push_back(charTostep(c));
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid direction string: " + c);
        }
    }
}

Step VisualSimulation::charTostep(char c) {
    if (c == 'N')
        return Step::North;
    if (c == 'E')
        return Step::East;
    if (c == 'S')
        return Step::South;
    if (c == 'W')
        return Step::West;
    if (c == 's')
        return Step::Stay;
    // (c == "F")
    return Step::Finish;
}

void VisualSimulation::loadParameters(House& h) {
    maxBatterySteps = h.getMaxBattery();
    batterySteps = maxBatterySteps;
    allowedNumberOfSteps = h.getMaxSteps();
    amountOfDirt = h.getAmountOfDirt();
    dockingStationLocation = h.getDockingStationLocation();
    robotLocation = dockingStationLocation;
}

std::string VisualSimulation::intToHouseElementString(int num) {
    switch (num)
    {
        case static_cast<int>(Elements::DockingStation):
            return infoRepMapping[Elements::DockingStation];
        case static_cast<int>(Elements::Wall):
            return infoRepMapping[Elements::Wall];
        default:
            return std::to_string(num);
    }
}

void VisualSimulation::loadHouseMapping(House& h) {
    std::vector<std::vector<int>> house_surface = h.getHouseSurface();
    for (const auto& row : house_surface) {
        std::vector<std::string> temp;
        for (int num : row) {
            temp.push_back(intToHouseElementString(num));
        }
        map.push_back(temp);
    }
}

void VisualSimulation::printHouse(bool printDir) {
    if (!firstRun) 
        clearMapArea(int(map.size()) + 3);

    for (int i = 0; i < int(map.size()); ++i) {
        for (int j = 0; j < int(map[i].size()); ++j) {
            if (i == robotLocation.first && j == robotLocation.second) {
                if (printDir)   {
                    std::cerr << elementToCodeColor[Elements::Direction] << " " << directionStringMap[currentStep] << " " << stringToCodeColor["Reset"];
                }         
                else {
                    std::cerr << elementToCodeColor[Elements::Robot] << " " << infoRepMapping[Elements::Robot] << " " << stringToCodeColor["Reset"];    
                }   
            } else {
                if (map[i][j] == infoRepMapping[Elements::Wall]) {
                    std::cerr << elementToCodeColor[Elements::Wall] << " " << map[i][j] << " " << stringToCodeColor["Reset"];
                }
                else if (map[i][j] == infoRepMapping[Elements::DockingStation])  {
                    std::cerr << elementToCodeColor[Elements::DockingStation] << " " << map[i][j] << " " << stringToCodeColor["Reset"];
                }
                else {
                    std::cerr << " " << map[i][j] << " ";
                }
            }

        }
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
    std::cerr.flush(); 
} 

void VisualSimulation::updateMap() {
    std::pair<int, int> dirElements = Common::stepMap.at(currentStep);
    robotLocation.first += dirElements.first;
    robotLocation.second += dirElements.second;
    
    if (currentStep == Step::Stay) {
        if (robotLocation == dockingStationLocation) {
            // In charge- no steps are decreasing, only increasing
            charge();
        }
        else {
            batterySteps--;
            clean();
        }
    }
    else {
        batterySteps--;
    }
    // Step has been made (I assume that no steps in battery counted here)
    allowedNumberOfSteps--;
}

void VisualSimulation::startSimulation() {
    firstRun = true;
    std::cerr << std::endl << "SIMULATION STARTS ..." << std::endl;

    printHouse(false);
    firstRun = false;
    // Hide the cursor
    std::cerr << "\033[?25l";
    for (Step step: steps) {
        currentStep = step;
        if (currentStep == Step::Finish) {
            break;
        }
        
        printHouse(true);

        std::this_thread::sleep_for(std::chrono::seconds(1));
        // "make the step" -> update map
        updateMap();
        printHouse(false);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cerr << std::endl << "SIMULATION ENDED" << std::endl;
    // Show the cursor
    std::cerr << "\033[?25h";
}

void VisualSimulation::charge() {
    batterySteps += (static_cast<double>(maxBatterySteps)/20);
    if (batterySteps > maxBatterySteps)
        batterySteps = maxBatterySteps;
}

void VisualSimulation::clean() {
    std::string cuurent_dirt = map[robotLocation.first][robotLocation.second];
    auto it = dirtLevelMapping.find(cuurent_dirt);
    if (it != dirtLevelMapping.end()) {
        map[robotLocation.first][robotLocation.second] = dirtLevelMapping[cuurent_dirt];
    } else {
        // Notice- shouldnt be zero!
        throw std::runtime_error("Invalid cuurent dirt: " + cuurent_dirt);
    }
}

void VisualSimulation::clearMapArea(int height) {
    std::cerr << "\033[" << height << "A";
    for (int i = 0; i < height; ++i) {
        std::cerr << "\033[2K\r"; // \r moves the cursor to the beginning of the line
    }
}

void VisualSimulation::changeInfoRepMapping(VisualConfigurationManager configM) {
    infoRepMapping = {
        {Elements::Wall, configM.getWallRep()},
        {Elements::DockingStation, configM.getStationRep()},
        {Elements::Robot, configM.getRobotRep()},
        {Elements::EmptyWall, " "}
    };
}

std::string VisualSimulation::getColorCode(std::string colorStr){
    auto it = stringToCodeColor.find(colorStr);
    if (it != dirtLevelMapping.end()) 
        return stringToCodeColor[colorStr];
    return stringToCodeColor["White"];
}

void VisualSimulation::changeColorMapping(VisualConfigurationManager configM) {
    std::map<Elements, std::string> string_to_string = configM.getColorMapping();
    elementToCodeColor = {
            {Elements::Robot, getColorCode(string_to_string[Elements::Robot])},
            {Elements::DockingStation, getColorCode(string_to_string[Elements::DockingStation])},
            {Elements::Direction, getColorCode(string_to_string[Elements::Direction])},
            {Elements::Wall, getColorCode(string_to_string[Elements::Wall])}
    };
}