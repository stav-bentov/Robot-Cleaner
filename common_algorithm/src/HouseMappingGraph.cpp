#include "../include/house_mapping_graph.h"
HouseMappingGraph::HouseMappingGraph() : tiles(),
                                        dockingStationLocation(0, 0), 
                                        currentLocation(0, 0), // Initialize with default values
                                        needToReturn(false),
                                        needToFinish(false),
                                        needToCharge(false),
                                        onDeterminedWayFromCharging(false),
                                        currentDistanceFromDocking(-1) {
        
    distanceFromDirt = -1;
    distanceFromUnkwon = -1;
    dirtyDst = {-1, -1};
    unkwonDst = {-1, -1};
    addTile(dockingStationLocation, Type::DockingStation);
    
}

void HouseMappingGraph::addTile(std::pair<int, int> location, Type t) {
    // Add tile for the first time
    if (tiles.find(location) == tiles.end()) {
        if (t != Type::DockingStation) {
            // Tile is added for the first time so we dont know its dirt
            tiles[location] = static_cast<int>(TilesType::UnknownDirt);
        }
    }
}

void HouseMappingGraph::addTile(Direction d, Type t) {
    std::pair<int, int> location = getRelativeLocation(d);
    // If vertex doesnt exist (accordint to its coordinate)- add it
    addTile(location, t);
}

std::pair<int, int> HouseMappingGraph::getRelativeLocation(Direction d) {
    std::pair<int, int> cord = Common::directionMap.at(d);
    std::pair<int, int> location = std::make_pair(currentLocation.first + cord.first, currentLocation.second + cord.second);
    return location;
}

bool HouseMappingGraph::isDockingStation(std::pair<int, int> location) const {
    return location == dockingStationLocation;
}

void HouseMappingGraph::setDirt(std::pair<int, int> location, int dirt) {
    if (tiles.find(location) != tiles.end()) {
        // Make sure set dirt to a floor (and not a wall or docking station)
        if (tiles[location] != static_cast<int>(TilesType::DockingStation)){
            tiles[location] = dirt;
        }
    }
}

void HouseMappingGraph::setDirt(int dirt) {
    visitedTiles.insert(currentLocation);
    if (dirt >= 0)
        setDirt(currentLocation, dirt);
}

void HouseMappingGraph::reduceDirt(Step s) {
    // Assumption- vertex currentLocation exist
    if (s == Step::Stay) {
        if (tiles[currentLocation] != static_cast<int>(TilesType::DockingStation)) { 
            // Clean here (not docking station)
            tiles[currentLocation]--;
        }
    }
}

void HouseMappingGraph::updateCurrentLocation(Step s) {
    // reduce dirt if needed
    reduceDirt(s);

    // Update location
    std::pair<int, int> stepElements = Common::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
}

Step HouseMappingGraph::getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps) {
    // If robot on way to clean- get from robotDeterminedPath the next step
    if (onWayToClean || needToReturn) {
        if (!robotDeterminedPath.empty()) {
            Step s = getStepFromSrcToDst(currentLocation, robotDeterminedPath.front());
            updateCurrentLocation(s);
            robotDeterminedPath.pop();
            return s;
        }
        // else- done robotDeterminedPath
        onWayToClean = false;
    }

    // Handle charging and return to docking station
    if (shouldStayCharging(batterySteps, maxBatterySteps, maxSteps)) {
        return Step::Stay; // or the appropriate Step based on the condition
    }

    if (shouldFinish()) {
        return Step::Finish;
    }

    // Check if we need to clean the current location
    if (shouldCleanCurrentLocation(batterySteps, maxSteps)) {
        updateCurrentLocation(Step::Stay);
        return Step::Stay;
    }
    
    // Handle cleaning, exploring, or finishing
    return decideNextStep(batterySteps, maxSteps);
}

/*
    Check if need to finish
*/
bool HouseMappingGraph::shouldFinish() {
    // if we are in docking and we need to finish OR we dont have any accessible tile to reach (dirt or unkwon- finish)
    return isDockingStation(currentLocation) && needToFinish;
}

/*
    Check if need to stay in docking and charge
*/
bool HouseMappingGraph::shouldStayCharging(int batterySteps, int maxBatterySteps, int maxSteps) {
    // Can't charge more
    if (batterySteps == maxBatterySteps) {
        needToCharge = false;
        return false;
    }

    if (needToFinish) {
        return false;
    }
    
    // Got to docking station because of needing did not have enogh steps- check if there is a need to stay charging
    if (needToReturn && isDockingStation(currentLocation)) {
        needToReturn = false;
        getPotentialDst(true);

        if (shouldNeedToFinish(batterySteps, maxBatterySteps, maxSteps, distanceFromDirt) &&
            shouldNeedToFinish(batterySteps, maxBatterySteps, maxSteps, distanceFromUnkwon)) {
            needToCharge = false;
            needToFinish = true;
            return false;
        }
    } 
    return needToCharge && isDockingStation(currentLocation) && batterySteps < maxBatterySteps && maxSteps > 0;
}


bool HouseMappingGraph::shouldCleanCurrentLocation(int batterySteps, int maxSteps) {
    if (tiles[currentLocation] > 0 && canCleanCurrentLocation(batterySteps, maxSteps)) {
        if (tiles[currentLocation] == 1) {
            // For calculating distance only once
            currentDistanceFromDocking = -1;
        }
        return true;
    }
    currentDistanceFromDocking = -1;
    return false;
}

bool HouseMappingGraph::canCleanCurrentLocation(int batterySteps, int maxSteps) {
    // Check if we have enough battery and steps to clean the current location and return to the docking station
    if (currentDistanceFromDocking == -1) {
        currentDistanceFromDocking = getDistanceFromDock(currentLocation);
    }
    return currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps);
}

bool HouseMappingGraph::shouldNeedToFinish(int batterySteps, int maxBatterySteps, int maxSteps, int distance){
    // Check if there is a point in stay charging
    if (distance != -1) {
        int stepsToDo = 2*distance;

        // If robot is fully charged- he can do the calening
        // +1 for the cleaning
        if (stepsToDo + 1 <= maxBatterySteps) {
            // Check regarding to maxSteps
            int stepsMissing = stepsToDo - batterySteps;
            // +1 for the cleaning
            int stepsToCharge = std::ceil((20.0 * (stepsMissing + 1)) / maxSteps);
            if (stepsToCharge + stepsToDo + 1 <= maxSteps) {
                return false;
            }
        }
    }
    return true;
}

bool HouseMappingGraph::enoghBatteryAndMaxSteps(int distanceFromDst, int DistanceBetweenDstAndDock, int batterySteps, int maxSteps) {
    return distanceFromDst + 1 + DistanceBetweenDstAndDock <= std::min(batterySteps, maxSteps);
}

bool HouseMappingGraph::canReachAndCleanDirt(int batterySteps, int maxSteps, int distanceBetweenDockAndDst) {
    int stepsToDstAndGetBack = distanceFromDirt + 1 + distanceBetweenDockAndDst;
    return stepsToDstAndGetBack <= std::min(batterySteps, maxSteps);
}

bool HouseMappingGraph::canExploreUnknown(int batterySteps, int maxSteps, int distanceBetweenDockAndDst) {
    int stepsToDstAndGetBack = distanceFromUnkwon + distanceBetweenDockAndDst;
    return stepsToDstAndGetBack <= std::min(batterySteps, maxSteps);
}

Step HouseMappingGraph::getStepToTarget(std::pair<int, int> target) {
    shortestPathToDstWithMaximumUnknown(currentLocation, target);
    if (!robotDeterminedPath.empty()) {
        Step s = getStepFromSrcToDst(currentLocation, robotDeterminedPath.front());
        updateCurrentLocation(s);
        robotDeterminedPath.pop();
        return s;
    }
    return Step::Finish;
}

Step HouseMappingGraph::directionToStep(Direction d) {
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

void HouseMappingGraph::getPotentialDst(bool priority) {
    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;

    // Init values
    distanceFromDirt = -1;
    distanceFromUnkwon = -1;
    distanceInGeneral = -1;
    dirtyDst = {-1, -1};
    unkwonDst = {-1, -1};
    generalDst = {-1, -1};

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

            if (priority) {
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
            }
            else {
                 if (tiles[location] == -1 || tiles[location] > 0) {
                    generalDst = location;
                    distanceInGeneral = depth;
                    break;
                }
            }
            updateQ(location, visited, q);
        }
        // Found dirt (this is the highest priorety- exit loop)
        if (priority && distanceFromDirt != -1 && distanceFromUnkwon != -1) {
            break;
        }
        // Found dirt (this is the highest priorety- exit loop)
        if (!priority && distanceInGeneral != -1) {
            break;
        }
        ++depth;
    }
}

int HouseMappingGraph::getDistanceFromDock(std::pair<int, int>& dst) {
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
            }

            updateQ(location, visited, q);
        }
        ++depth;
    }
    // Wont heppen
    return -1;
}

void HouseMappingGraph::shortestPathToDstWithMaximumUnknown(std::pair<int, int> start, std::pair<int, int> dst) {
    std::queue<std::tuple<std::pair<int, int>, std::vector<std::pair<int, int>>, int, int>> q; // Tuples of (current position, path, negOneCount, distance)
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> visited; // position to (distance, maxNegOneCount)
    std::vector<std::pair<int, int>> result;
    int maxNegOneCount = 0;
    q.push({start, {}, 0, 0});
    visited[start] = {0, 0};

    while (!q.empty()) {
        int size = q.size();
        for (int i = 0; i < size; ++i) {
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
    }

    for (const auto& p : result) {
      //  std::cout <<thread<< " (" << p.first << ", " << p.second << ") ";
        robotDeterminedPath.push(p);
    }
}

void HouseMappingGraph::getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack) {
    std::pair<int, int> current = dst;
    std::pair<int, int> prev = dst;
    int rowDiff = 0;
    int colDiff = 0;
    while (current != currentLocation) {
        // std::cout << thread <<" current = " <<current.first << ", " << current.second << std::endl;
        prev = current;
        current = parent.at(current);
        rowDiff = current.first - prev.first;
        colDiff = current.second - prev.second;
        fillStack.push(getStepByDiff(rowDiff, colDiff));
    }
    //printStack(fillStack);

}

void HouseMappingGraph::printStack(std::stack<Step> s) {
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

Step HouseMappingGraph::getStepFromSrcToDst(std::pair<int, int> src, std::pair<int, int> dst) {
    int rowDiff = src.first - dst.first;
    int colDiff = src.second - dst.second;
    return getStepByDiff(rowDiff, colDiff);
}

Step HouseMappingGraph::getStepByDiff(int diffrenceRow, int diffrenceCol) {
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

bool HouseMappingGraph::isVisitedInCurrentLocation() const {
    if (visitedTiles.find(currentLocation) == visitedTiles.end()) {
        return false;
    }
    return true;
}

