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
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // Add tile for the first time
    if (tiles.find(location) == tiles.end()) {
        if (t != Type::DockingStation) {
            std::cout <<thread<< " addTile: " <<location.first << ", " << location.second<< std::endl;
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
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // Assumption- vertex currentLocation exist
    if (s == Step::Stay) {
        if (tiles[currentLocation] != static_cast<int>(TilesType::DockingStation)) { 
            std::cout <<thread<< " Step is stay and we are not in docking- - reduce dirt" << std::endl;
            // Clean here (not docking station)
            tiles[currentLocation]--;
            std::cout << thread<<" Dirt in relative location: " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        }
    }
}

void HouseMappingGraph::updateCurrentLocation(Step s) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    std::cout << thread<<" updateCurrentLocation" << std::endl;
    // reduce dirt if needed
    reduceDirt(s);

    // Update location
    std::cout << thread<<" currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
    std::pair<int, int> stepElements = Common::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
    std::cout << thread<<" Updated to: currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
}

Step HouseMappingGraph::getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "]: ";
    std::cout << thread << "HouseMappingGraph:getStepFromMapping currentLocation: " << currentLocation.first << ", " << currentLocation.second << std::endl;

    // If Im on way to clean- get from robotDeterminedPath the next step
    if (onWayToClean || needToReturn) {
        std::cout <<thread<< "if (onWayToClean)" << std::endl;
        if (!robotDeterminedPath.empty()) {
            std::cout <<thread<< "!robotDeterminedPath.empty()" << std::endl;
            Step s = getStepFromSrcToDst(currentLocation, robotDeterminedPath.front());
            updateCurrentLocation(s);
            robotDeterminedPath.pop();
            return s;
        }
        // else- done robotDeterminedPath
        std::cout <<thread<< "onWayToClean = false;" << std::endl;
        onWayToClean = false;

        // If Im on docking because I dont have enogh battery steps -charge
        if (needToReturn && !needToFinish) {
            calculateChargingTime(batterySteps, maxBatterySteps, maxSteps);
            needToReturn = false;
        }
    }

    // Added //
    if (onDeterminedWayFromCharging) {
        std::cout <<"onDeterminedWayFromCharging" << std::endl;
        // Charge required amount
        if (stepsToCharge > 0) {
            std::cout <<"stepsToCharge > 0" << std::endl;
            stepsToCharge--;
            return Step::Stay;
        }

        // Clean tiles in path
        if (tiles[currentLocation] > 0) {
            std::cout <<thread<< "tiles[currentLocation] > 0" << std::endl;
            updateCurrentLocation(Step::Stay);
            return Step::Stay;
        }
        // if no tile to clean- continue in determined path
        if (!robotDeterminedPath.empty()) {
            std::cout <<thread<< "!robotDeterminedPath.empty()" << std::endl;
            Step s = getStepFromSrcToDst(currentLocation, robotDeterminedPath.front());
            updateCurrentLocation(s);
            robotDeterminedPath.pop();
            return s;
        }
        // Done determined path- continue
        std::cout <<thread<< "onDeterminedWayFromCharging = false;" << std::endl;
        onDeterminedWayFromCharging = false;
    }
    // Added //

    if (shouldFinish()) {
        std::cout <<thread<< "shouldFinish()- return finish" << std::endl;
        return Step::Finish;
    }

    // Handle charging and return to docking station
    /*if (shouldStayCharging(batterySteps, maxBatterySteps, maxSteps)) {
        std::cout <<thread<< "shouldStayCharging(batterySteps, maxBatterySteps, maxSteps)- return stay" << std::endl;
        return Step::Stay; // or the appropriate Step based on the condition
    }*/

    // Check if we need to clean the current location
    if (shouldCleanCurrentLocation(batterySteps, maxSteps)) {
        std::cout <<thread<< "shouldCleanCurrentLocation(batterySteps, maxSteps)- return stay" << std::endl;
        updateCurrentLocation(Step::Stay);
        return Step::Stay;
    }
    
    // Handle cleaning, exploring, or finishing
    return decideNextStep(batterySteps, maxSteps);
}


    // Added //
void HouseMappingGraph::calculateChargingTime(int batterySteps, int maxBatterySteps, int maxSteps) {
    std::string thread = " in thread calculateChargingTime [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "]: ";
    std::cout <<"calculateChargingTime" << std::endl;
    std::cout <<batterySteps << std::endl;
    std::cout <<maxBatterySteps << std::endl;
    std::cout <<maxSteps << std::endl;
    std::queue<std::tuple<std::pair<int, int>, std::vector<std::pair<int, int>>, int, int>> q; // Tuples of (current position, path, max clean, distance)
    std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash> visited; // position to (distance, max clean)
    int depth = 0;
    int stepsForMission = 0;
    stepsToCharge = 0;

    // currentLocation is docking Station...
    q.push({currentLocation, {}, 0, 0});
    visited[currentLocation] = {0, 0};
    int currentDirtToBeCleaned = 0;
    std::vector<std::pair<int, int>> result;
    int cleanAmount = -1;

    // Create copy of Tiles with random dirt for unkwon tiles
    std::unordered_map<std::pair<int, int>, int, pair_hash> newTiles;
    std::cout <<"newTiles" <<std::endl;

    for (const auto& [key, value] : tiles) {
        int newValue = value;
        if (value == -1) {
            newValue = std::rand() % 10; // Random value between 0 and 9
        }
        if (value == -2) {
            newValue = 0; // Random value between 0 and 9
        }
        newTiles[key] = newValue;
        std::cout <<"newTiles[" << key.first  << ", " << key.second << "] = " << newValue <<std::endl;
    }


    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            auto [current, path, maxDirt, currentDistance] = q.front();
            std::cout <<"current = " <<current.first << ", "<<current.second<< " maxDirt = " << maxDirt << " currentDistance " << currentDistance <<std::endl;
            q.pop();

            if (current != dockingStationLocation) {
                int currentDirt = newTiles.at(current);
                std::cout <<"currentDirt = " <<currentDirt <<std::endl;
                // Calculate number of steps and clean until current tile
                int stepsToAndFromDst = 2*currentDistance + maxDirt - currentDirt;
                // Check if battery is enogh for this path...
                if (stepsToAndFromDst + 1 <= maxBatterySteps && stepsToAndFromDst + 1 <= maxSteps) {
                    // if current battery steps is enogh- choose the path without charging
                    if (stepsToAndFromDst + 1 <= batterySteps) {
                        // clean maxDirt minus dirt of current tile, and add the maximum amount of dirt that can be cleaned in current tile.
                        cleanAmount = maxDirt - currentDirt + std::min(batterySteps - stepsToAndFromDst, currentDirt);
                        if (cleanAmount > currentDirtToBeCleaned) {
                            // Choose path
                            currentDirtToBeCleaned = cleanAmount;
                            result = path;
                            stepsToCharge = 0;
                        }
                    }
                    else {
                        // For each option of dirt to clean- check if possibole
                        for (int potentialDirtToClean = currentDirt; potentialDirtToClean >=1; potentialDirtToClean--) {

                            // steps to have to clean at least potentialDirtToClean amount of dirt in current tile clean...
                            int neededAmountOfStepsToHave = (stepsToAndFromDst + potentialDirtToClean) - batterySteps;
                            // To get neededAmountOfStepsToHave steps from charging, I need to charge (20*neededAmount)/maxSteps, 
                            // because each step on docking gives me maxSteps/20 steps.
                            int stepsForChargingToGetSteps = std::ceil((20.0 * neededAmountOfStepsToHave) / maxBatterySteps);
                            int StepsToChargeAndMission = stepsForChargingToGetSteps + neededAmountOfStepsToHave;

                            if (maxSteps >= StepsToChargeAndMission) {
                                cleanAmount = maxDirt - currentDirt + potentialDirtToClean;
                                if (cleanAmount > currentDirtToBeCleaned) {
                                    // Choose path
                                    currentDirtToBeCleaned = cleanAmount;
                                    result = path;
                                    stepsForMission = StepsToChargeAndMission;
                                    stepsToCharge = stepsForChargingToGetSteps;
                                    break;

                                }
                            }
                        }
                    }
                }
            }
            
            // Consider all 4 possible directions
            for (const auto& entry : Common::directionMap) {
                std::pair<int, int> nextTile = {current.first + entry.second.first, current.second + entry.second.second};

                if (newTiles.find(nextTile) != newTiles.end()) {

                    int nextTileDirt = newTiles.at(nextTile); // Ensure non-negative dirt
                    int newMaxDirt = maxDirt + nextTileDirt;
                    int newDistance = currentDistance + 1;


                    bool shouldEnqueue = visited.find(nextTile) == visited.end() || 
                                    visited[nextTile].first > newDistance || 
                                    (visited[nextTile].first == newDistance && visited[nextTile].second < newMaxDirt);
                    
                    if (shouldEnqueue) {
                        // Update visited with new distance and max dirt count
                        visited[nextTile] = {newDistance, newMaxDirt};

                        // Create new path
                        std::vector<std::pair<int, int>> newPath = path;
                        newPath.push_back(nextTile);

                        // Enqueue the next position
                        q.push({nextTile, newPath, newMaxDirt, newDistance});
                    }
                }
            }
        }
        ++depth;
    }

    for (const auto& p : result) {
        robotDeterminedPath.push(p);
    }
    if (currentDirtToBeCleaned != 0) {
        onDeterminedWayFromCharging = true;
    }

}
    // Added //


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
        return false;
    }

    // If we need to charge and in docking- calculate how much we need to charge
    if (needToCharge && isDockingStation(currentLocation)){ 
        calculateChargingTime(batterySteps, maxBatterySteps,maxSteps);
    }
    return isDockingStation(currentLocation) && needToCharge && batterySteps < maxBatterySteps && maxSteps > 0;
}


bool HouseMappingGraph::shouldCleanCurrentLocation(int batterySteps, int maxSteps) {
    if (tiles[currentLocation] > 0 && canCleanCurrentLocation(batterySteps, maxSteps)) {
        if (tiles[currentLocation] == 1) {
            // For calculating distance only once
            currentDistanceFromDocking = -1;
        }
        return true;
    }
    return false;
}

bool HouseMappingGraph::canCleanCurrentLocation(int batterySteps, int maxSteps) {
    // Check if we have enough battery and steps to clean the current location and return to the docking station
    if (currentDistanceFromDocking == -1) {
        currentDistanceFromDocking = getDistanceFromDock(currentLocation);
    }
    return currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps);
}

void HouseMappingGraph::shouldNeedToFinish(int maxSteps, int distanceFromUnkwon, int distanceFromDirt, int distanceOfUnkownFromDock, int distanceOfDirtFromDock){
    // When to set finish?
    // No tiles to explore
    // tiles that can be explored are too far for max steps
    if (distanceFromUnkwon == -1 && distanceFromDirt == -1) {
        needToFinish = true;
    } else {
        bool enoughSteps = false;

        if (distanceFromDirt != -1 && haveEnoghMaxSteps(maxSteps, distanceOfDirtFromDock)) {
            enoughSteps = true;
        }

        if (distanceFromUnkwon != -1 && haveEnoghMaxSteps(maxSteps, distanceOfUnkownFromDock)) {
            enoughSteps = true;
        }

        if (!enoughSteps) {
            needToFinish = true;
        }
    }
}

bool HouseMappingGraph::haveEnoghMaxSteps(int maxSteps, int distanceBetweenDockAndDst) {
    return distanceBetweenDockAndDst + 1 + distanceFromDirt <= maxSteps;
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

void HouseMappingGraph::getDistanceFromDockingAndPotentialDst(int& distanceFromDocking) {
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
        // Found dirt (this is the highest priorety- exit loop)
        if (distanceFromDirt != -1 && distanceFromDocking != -1) {
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
                break;
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
        robotDeterminedPath.push(p);
    }
}

void HouseMappingGraph::getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack) {
    std::pair<int, int> current = dst;
    std::pair<int, int> prev = dst;
    int rowDiff = 0;
    int colDiff = 0;
    while (current != currentLocation) {
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
            break;
        case Step::West:
            break;
        case Step::North:
            break;
        case Step::East:
            break;
        case Step::Stay:
            break;
        case Step::Finish:
            break;
        }
        // Pop the top element
        s.pop();
        // Push it onto the temporary stack
        tempStack.push(top);
    }

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
    return Step::Stay;
}

bool HouseMappingGraph::isVisitedInCurrentLocation() const {
    if (visitedTiles.find(currentLocation) == visitedTiles.end()) {
        return false;
    }
    return true;
}

