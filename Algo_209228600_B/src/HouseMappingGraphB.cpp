#include "../include/house_mapping_graph_B.h"

HouseMappingGraphB::HouseMappingGraphB() : tiles(),
                                           dockingStationLocation(0, 0), 
                                           currentLocation(0, 0), // Initialize with default values
                                           needToReturn(false),
                                           needToFinish(false),
                                           needToCharge(false),
                                           currentDistanceFromDocking(-1) {
        
    distanceFromDirt = -1;
    distanceFromUnkwon = -1;
    dirtyDst = {-1, -1};
    unkwonDst = {-1, -1};
    addTile(dockingStationLocation, Type::DockingStation);
}

void HouseMappingGraphB::addTile(std::pair<int, int> location, Type t) {
    // Add tile for the first time
    if (tiles.find(location) == tiles.end()) {
        if (t != Type::DockingStation) {
            std::cout << "addTile: " <<location.first << ", " << location.second<< std::endl;
            // Tile is added for the first time so we dont know its dirt
            tiles[location] = static_cast<int>(TilesType::UnknownDirt);
        }
    }
}

void HouseMappingGraphB::addTile(Direction d, Type t) {
    std::pair<int, int> location = getRelativeLocation(d);
    // If vertex doesnt exist (accordint to its coordinate)- add it
    addTile(location, t);
}

std::pair<int, int> HouseMappingGraphB::getRelativeLocation(Direction d) {
    std::pair<int, int> cord = Common::directionMap.at(d);
    std::pair<int, int> location = std::make_pair(currentLocation.first + cord.first, currentLocation.second + cord.second);
    return location;
}

bool HouseMappingGraphB::isDockingStation(std::pair<int, int> location) const {
    return location == dockingStationLocation;
}

void HouseMappingGraphB::setDirt(std::pair<int, int> location, int dirt) {
    if (tiles.find(location) != tiles.end()) {
        // Make sure set dirt to a floor (and not a wall or docking station)
        if (tiles[location] != static_cast<int>(TilesType::DockingStation)){
            tiles[location] = dirt;
        }
    }
}

void HouseMappingGraphB::setDirt(int dirt) {
    if (dirt >= 0)
        setDirt(currentLocation, dirt);
}

void HouseMappingGraphB::reduceDirt(Step s) {
    // Assumption- vertex currentLocation exist
    if (s == Step::Stay) {
        if (tiles[currentLocation] != static_cast<int>(TilesType::DockingStation)) { 
            std::cout << "Step is stay and we are not in docking- - reduce dirt" << std::endl;
            // Clean here (not docking station)
            tiles[currentLocation]--;
            std::cout << "Dirt in relative location: " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        }
    }
}

void HouseMappingGraphB::updateCurrentLocation(Step s) {
    std::cout << "updateCurrentLocation" << std::endl;
    // reduce dirt if needed
    reduceDirt(s);

    // Update location
    std::cout << "currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
    std::pair<int, int> stepElements = Common::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
    std::cout << "Updated to: currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
}

/*
    Run bfs and calculate next step
*/
Step HouseMappingGraphB::getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps) {
    std::cout << "getStepFromMapping currentLocation: " << currentLocation.first << ", "<< currentLocation.second<< std::endl;
    Step s;

    // Check if was sent to clean current location and clean if there is enogh battery
    // If there is not enogh battery- return to docking
    if (currentDistanceFromDocking != -1) {
        // Check if need to clean and if can clean
        if (tiles[currentLocation] > 0) {
            // Check if can clean
            if (currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps)) {
                updateCurrentLocation(Step::Stay);
                return Step::Stay;
            }
            else { // Cannot clean current location don't have enogh steps- return
                shortestPathToDstWithMaximumUnknown(currentLocation, dockingStationLocation);
                needToReturn = true;
                currentDistanceFromDocking = -1;
                return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
            }
        }
        // Done cleaning current location- dont need this information
        currentDistanceFromDocking = -1;
    }

    if (needToReturn) {
        std::cout << "needToReturn" << std::endl;

        std::pair<int, int> dst = robotDeterminedPath.front();
        robotDeterminedPath.pop();

        std::cout << "from: currentLocation = " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        std::cout << "to: dst = " <<dst.first << ", " << dst.second << std::endl;
        s = getStepFromSrcToDst(currentLocation, dst);
        updateCurrentLocation(s);

        // Done- with s will be in the docking station
        if (robotDeterminedPath.empty()) {
            needToReturn = false;
            needToCharge = true;
        }
        return s;
    }

    if (needToFinish) {
        return Step::Finish;
    }

    if (onWayToClean) {
        std::cout << "onWayToClean" << std::endl;

        std::pair<int, int> dst = robotDeterminedPath.front();
        robotDeterminedPath.pop();

        std::cout << "from: currentLocation = " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        std::cout << "to: dst = " <<dst.first << ", " << dst.second << std::endl;
        s = getStepFromSrcToDst(currentLocation, dst);
        updateCurrentLocation(s);

        // Done- with s will be in the docking station
        if (robotDeterminedPath.empty()) {
            onWayToClean = false;
        }
        return s;
    }

    // Need to decide where to go and if to clean
    int distanceFromDocking = 0;
    getDistanceFromDockingAndPotentialDst(distanceFromDocking);
    std::cout << "dirtyDst we GOT: " << dirtyDst.first << ", " << dirtyDst.second << std::endl;
    std::cout << "distanceFromDirt: " << distanceFromDirt << std::endl;

    std::cout << "unkwonDst we GOT: " << unkwonDst.first << ", " << unkwonDst.second << std::endl;
    std::cout << "distanceFromUnkwon: " << distanceFromUnkwon << std::endl;

    std::cout << "distanceFromDocking: " << distanceFromDocking << std::endl;
    std::cout << "batterySteps: " << batterySteps << std::endl;
    std::cout << "maxSteps: " << maxSteps << std::endl;


    // Charge until max battery
    if (isDockingStation(currentLocation) && needToCharge && batterySteps < maxBatterySteps) {
        // Can't achive full charge with current amount of steps
        if (maxSteps < maxBatterySteps - batterySteps) {
            return Step::Finish;
        }
        return Step::Stay;
    }
    // Check if there is a point in charging battery
    /*if (isDockingStation(currentLocation)) {
        bool canGetToDirt = false;
        bool canGetToUnknwon = false;
        if (distanceFromDirt != -1) {
            // Check if dst is reachable with amount available steps
            int stepsToCleanAndGetBack = 2*distanceFromDirt + 1;
            int stepsChargeGoCleanAndGetBack = 2*(stepsToCleanAndGetBack);

            // If full charge can make the job
            if (stepsToCleanAndGetBack <= maxBatterySteps) {
                // Check if there is enogh steps to charge and do this
                if (stepsChargeGoCleanAndGetBack <= maxSteps) {
                    std::cout << "canGetToDirt: " << std::endl;
                    canGetToDirt = true;
                }
            }
        }

        if (!canGetToDirt) {
            if (distanceFromUnkwon != -1) {
                // Check if dst is reachable with amount available steps
                int stepsToCleanAndGetBack = 2*distanceFromUnkwon + 1;
                int stepsChargeGoCleanAndGetBack = 2*(stepsToCleanAndGetBack);

                // If full charge can make the job
                if (stepsToCleanAndGetBack <= maxBatterySteps) {
                    // Check if there is enogh steps to charge and do this
                    if (stepsChargeGoCleanAndGetBack <= maxSteps) {
                        std::cout << "canGetToUnknwon: " << std::endl;
                        canGetToUnknwon = true;
                    }
                }
            }
        }

        if (!canGetToDirt && !canGetToUnknwon) {
            std::cout << "!canGetToDirt && !canGetToUnknwon: " << std::endl;
            return Step::Finish;
        }
    
    }*/

    // Check if current location is dirty
    if (tiles[currentLocation] > 0) {
        std::cout << "tiles[currentLocation] > 0: " << std::endl;
        // This is the closest dirt- call function again to clean
        currentDistanceFromDocking = distanceFromDirt;
        return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
    }

    // Check if we can clean with current battery steps
    if (distanceFromDirt != -1) {
        // Prefere dirt!
        // Check if there is enogh to clean dirt (go to dst, clean 1 and then get back to docking)
        int stepsToDstAndGetBack = distanceFromDirt + 1 + getDistanceFromDock(dirtyDst);
        if (stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)) {
            std::cout << "Prefere dirt!" << std::endl;
            shortestPathToDstWithMaximumUnknown(currentLocation, dirtyDst);
            onWayToClean = true;
            return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
        }
    }
    if (distanceFromUnkwon != -1) {
        std::cout << "got unknown" << std::endl;
        // No dirt can be clean/ found- prefere unkwon
        int stepsToDstAndGetBack = distanceFromUnkwon + getDistanceFromDock(unkwonDst);
        if (stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)) {
            shortestPathToDstWithMaximumUnknown(currentLocation, unkwonDst);
            onWayToClean = true;
            return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
        }
    }
    
    // No dirt/ unkwon is found- return to docking and finish
    std::cout << "no tile to explore" << std::endl;
    shortestPathToDstWithMaximumUnknown(currentLocation, dockingStationLocation);
    needToReturn = true;
    needToFinish = true;
    return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
}

Step HouseMappingGraphB::directionToStep(Direction d) {
    Step s;
    //North, East, South, West
    switch (d)
    {
        case Direction::North:
            s = Step::North;
            break;
        case Direction::East:
            s = Step::East;
            break;
        case Direction::South:
            s = Step::South;
            break;
        case Direction::West:
            s = Step::West;
            break;
    }
    return s;
}

/*
    insert unvisited/ non-wall neighbors to queue
*/
void HouseMappingGraphB::updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, 
                                std::queue<std::pair<int, int>>& q) {
    for (const auto& entry : Common::directionMap) {  
        std::pair<int, int> neiLocation = {location.first + entry.second.first, location.second + entry.second.second};

        // If neiLocation exist
        if (tiles.find(neiLocation) != tiles.end()) {
            // Not a wall- check if not visited, if not add to queue
            if (visited.find(neiLocation) == visited.end()) {
                visited.insert(neiLocation);
                q.push(neiLocation);
            }
        }
    }
}

void HouseMappingGraphB::getDistanceFromDockingAndPotentialDst(int& distanceFromDocking) {
    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    // Init values
    distanceFromDirt = -1;
    distanceFromUnkwon = -1;
    dirtyDst = {-1, -1};
    unkwonDst = {-1, -1};

    int depth = 0;
    int amountOfDirt = 0;
    // Set queue
    q.push(currentLocation);
    visited.insert(currentLocation);

    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();

            // Distance from docking station is depth
            if (isDockingStation(location)) {
                distanceFromDocking = depth;
            }

            // Get closest tile with maximum dirt
            if (tiles[location] > 0 && tiles[location] > amountOfDirt && (distanceFromDirt == -1 || depth == distanceFromDirt)) {
                dirtyDst = location;
                distanceFromDirt = depth;
                amountOfDirt = tiles[location];
            }

            // Get closest tile with unkwon dirt
            if (tiles[location] == static_cast<int>(TilesType::UnknownDirt) && (distanceFromUnkwon == -1 || depth == distanceFromUnkwon)) {
                unkwonDst = location;
                distanceFromUnkwon = depth;
            }
            updateQ(location, visited, q);
        }
        // Found both potential dst
        if (distanceFromUnkwon != -1 && distanceFromDirt != -1) {
            break;
        }
        ++depth;
    }
}

int HouseMappingGraphB::getDistanceFromDock(std::pair<int, int>& dst) {
    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> parent;
    int depth = 0;

    // Set queue
    q.push(dockingStationLocation);
    visited.insert(dockingStationLocation);

    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();
            
            // Distance from docking station is depth
            if (location == dst) {
                return depth;
                break;
            }

            updateQ(location, visited, q);
        }
        ++depth;
    }
    // Wont heppen
    return -1;
}

void HouseMappingGraphB::shortestPathToDstWithMaximumUnknown(std::pair<int, int> start, std::pair<int, int> dst) {
   if (robotDeterminedPath.size() > 0) {
        throw std::runtime_error("!!!robotDeterminedPath.size() > 0");
    }
    std::queue<std::tuple<std::pair<int, int>, std::vector<std::pair<int, int>>, int, int>> q; // Tuples of (current position, path, negOneCount, distance)
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> visited; // position to (distance, maxNegOneCount)
    std::vector<std::pair<int, int>> result;
    int maxNegOneCount = 0;
    q.push({start, {}, 0, 0});
    visited[start] = {0, 0};

    while (!q.empty()) {
        auto [current, path, negOneCount, currentDistance] = q.front();
        q.pop();

        // Destination is reached
        if (current == dst) {
            if (negOneCount > maxNegOneCount || (negOneCount == maxNegOneCount && result.empty())) {
                maxNegOneCount = negOneCount;
                result = path;
            }
            continue; // Continue exploring other paths with the same distance
        }

        // Consider all 4 possible directions
        for (const auto& entry : Common::directionMap) {
            std::pair<int, int> nextTile = {current.first + entry.second.first, current.second + entry.second.second};

            if (tiles.find(nextTile) != tiles.end()) {
                int newNegOneCount = negOneCount + (tiles.at(nextTile) == -1 ? 1 : 0);
                int newDistance = currentDistance + 1;

                if (visited.find(nextTile) == visited.end() || 
                    visited[nextTile].first > newDistance || 
                    (visited[nextTile].first == newDistance && visited[nextTile].second < newNegOneCount)) {
                    
                    // Update visited with new distance and max negOne count
                    visited[nextTile] = {newDistance, newNegOneCount};

                    // Create new path
                    std::vector<std::pair<int, int>> newPath = path;
                    newPath.push_back(nextTile);

                    // Enqueue the next position
                    q.push({nextTile, newPath, newNegOneCount, newDistance});
                }
            }
        }
    }

    for (const auto& p : result) {
        std::cout << "(" << p.first << ", " << p.second << ") ";
        robotDeterminedPath.push(p);
    }
}

void HouseMappingGraphB::getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack) {
    std::pair<int, int> current = dst;
    std::pair<int, int> prev = dst;
    int rowDiff = 0;
    int colDiff = 0;
    while (current != currentLocation) {
        std::cout << "current = " <<current.first << ", " << current.second << std::endl;
        prev = current;
        current = parent.at(current);
        rowDiff = current.first - prev.first;
        colDiff = current.second - prev.second;
        fillStack.push(getStepByDiff(rowDiff, colDiff));
    }
    printStack(fillStack);
    std::cout << "getStepsFromParent" << std::endl;

}

void HouseMappingGraphB::printStack(std::stack<Step> s) {
    std::cout << "printStack" << std::endl;
    std::stack<Step> tempStack;
    while (!s.empty()) {
        // Get the top element
        Step top = s.top();
        // Print it
        switch (top)
        {
        case Step::South:
            std::cout << "South" << " ";
            break;
        case Step::West:
            std::cout << "West" << " ";
            break;
        case Step::North:
            std::cout << "North" << " ";
            break;
        case Step::East:
            std::cout << "East" << " ";
            break;
        case Step::Stay:
            std::cout << "Stay" << " ";
            break;
        case Step::Finish:
            std::cout << "Finish" << " ";
            break;
        }
        // Pop the top element
        s.pop();
        // Push it onto the temporary stack
        tempStack.push(top);
    }
    std::cout << std::endl;

    // Restore the original stack
    while (!tempStack.empty()) {
        s.push(tempStack.top());
        tempStack.pop();
    }
}

Step HouseMappingGraphB::getStepFromSrcToDst(std::pair<int, int> src, std::pair<int, int> dst) {
    int rowDiff = src.first - dst.first;
    int colDiff = src.second - dst.second;
    return getStepByDiff(rowDiff, colDiff);
}

Step HouseMappingGraphB::getStepByDiff(int diffrenceRow, int diffrenceCol) {
    if (diffrenceRow == -1 && diffrenceCol == 0) {
        return Step::South;
    }
    if (diffrenceRow == 1 && diffrenceCol == 0) {
        return Step::North;
    }
    if (diffrenceRow == 0 && diffrenceCol == -1) {
        return Step::East;
    }
    if (diffrenceRow == 0 && diffrenceCol == 1) {
        return Step::West;
    }
    // diffrenceRow == 0 && diffrenceCol == 0) 
    return Step::Stay;
    
}