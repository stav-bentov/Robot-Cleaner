#include "../include/house_mapping_graph.h"
#include <thread>
HouseMappingGraph::HouseMappingGraph() : tiles(),
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

void HouseMappingGraph::addTile(std::pair<int, int> location, Type t) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // Add tile for the first time
    if (tiles.find(location) == tiles.end()) {
        if (t != Type::DockingStation) {
            // std::cout <<thread<< " addTile: " <<location.first << ", " << location.second<< std::endl;
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
            // std::cout <<thread<< " Step is stay and we are not in docking- - reduce dirt" << std::endl;
            // Clean here (not docking station)
            tiles[currentLocation]--;
            // std::cout << thread<<" Dirt in relative location: " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        }
    }
}

void HouseMappingGraph::updateCurrentLocation(Step s) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // std::cout << thread<<" updateCurrentLocation" << std::endl;
    // reduce dirt if needed
    reduceDirt(s);

    // Update location
    // std::cout << thread<<" currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
    std::pair<int, int> stepElements = Common::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
    // std::cout << thread<<" Updated to: currentLocation.first: "<<currentLocation.first << "currentLocation.second: "<<currentLocation.second << std::endl;
}

/*
    Run bfs and calculate next step
*/
/*Step HouseMappingGraph::getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // std::cout << thread<<" getStepFromMapping currentLocation: " << currentLocation.first << ", "<< currentLocation.second<< std::endl;
    Step s;

    // Check if was sent to clean current location and clean if there is enogh battery
    // If there is not enogh battery- return to docking
    if (currentDistanceFromDocking != -1) {
        // std::cout << thread<<" currentDistanceFromDocking != -1"<< std::endl;

        // Check if need to clean and if can clean
        if (tiles[currentLocation] > 0) {
            // std::cout << thread<<" tiles[currentLocation] > 0"<< std::endl;
            // Check if can clean
            if (currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps)) {
                // std::cout << thread<<" currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps)"<< std::endl;
                updateCurrentLocation(Step::Stay);
                return Step::Stay;
            }
            else { // Cannot clean current location don't have enogh steps- return
                // std::cout << thread<<" ELSE currentDistanceFromDocking + 1 <= std::min(batterySteps, maxSteps)"<< std::endl;
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
        // std::cout << thread<<" needToReturn" << std::endl;

        std::pair<int, int> dst = robotDeterminedPath.front();
        robotDeterminedPath.pop();

        // std::cout << thread<<" from: currentLocation = " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        // std::cout << thread<<" to: dst = " <<dst.first << ", " << dst.second << std::endl;
        s = getStepFromSrcToDst(currentLocation, dst);
        updateCurrentLocation(s);

        // Done- with s will be in the docking station
        if (robotDeterminedPath.empty()) {
            // std::cout << thread<<" robotDeterminedPath.empty()"<< std::endl;
            // std::cout << thread<<" needToReturn = false needToCharge = true; "<< std::endl;
            needToReturn = false;
            needToCharge = true;
        }
        return s;
    }

    if (needToFinish) {
        return Step::Finish;
    }

    if (onWayToClean) {
        // std::cout << thread<<" if (onWayToClean)" << std::endl;

        std::pair<int, int> dst = robotDeterminedPath.front();
        robotDeterminedPath.pop();

        // std::cout << thread<<" from: currentLocation = " <<currentLocation.first << ", " << currentLocation.second << std::endl;
        // std::cout << thread<<" to: dst = " <<dst.first << ", " << dst.second << std::endl;
        s = getStepFromSrcToDst(currentLocation, dst);
        updateCurrentLocation(s);

        // Done- with s will be in the docking station
        if (robotDeterminedPath.empty()) {
            // std::cout << thread<<" robotDeterminedPath.empty()"<< std::endl;
            // std::cout << thread<<" onWayToClean = false"<< std::endl;
            onWayToClean = false;
        }
        return s;
    }

    // Need to decide where to go and if to clean
    int distanceFromDocking = 0;
    getDistanceFromDockingAndPotentialDst(distanceFromDocking);
    // std::cout <<thread<< " dirtyDst we GOT: " << dirtyDst.first << ", " << dirtyDst.second << std::endl;
    // std::cout << thread<<" distanceFromDirt: " << distanceFromDirt << std::endl;

    // std::cout << thread<<" unkwonDst we GOT: " << unkwonDst.first << ", " << unkwonDst.second << std::endl;
    // std::cout << thread<<" distanceFromUnkwon: " << distanceFromUnkwon << std::endl;

    // std::cout << thread<<" distanceFromDocking: " << distanceFromDocking << std::endl;
    // std::cout << thread<<" batterySteps: " << batterySteps << std::endl;
    // std::cout << thread<<" maxSteps: " << maxSteps << std::endl;


    // Charge until max battery
    if (isDockingStation(currentLocation) && needToCharge && batterySteps < maxBatterySteps) {
        // std::cout <<thread<< " isDockingStation(currentLocation) && needToCharge && batterySteps < maxBatterySteps" << std::endl;
        // Can't achive full charge with current amount of steps
        if (maxSteps < maxBatterySteps - batterySteps) {
            // std::cout <<thread<< " maxSteps < maxBatterySteps - batterySteps): Step::Finish" << std::endl;
            return Step::Finish;
        }
        return Step::Stay;
    }

    // Check if current location is dirty
    if (tiles[currentLocation] > 0) {
        // std::cout <<thread<< " tiles[currentLocation] > 0: " << std::endl;
        // This is the closest dirt- call function again to clean
        currentDistanceFromDocking = distanceFromDirt;
        // std::cout <<thread<< "getStepFromMapping" << std::endl;
        return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
    }

    // Check if we can clean with current battery steps
    if (distanceFromDirt != -1) {
        // Prefere dirt!
        // Check if there is enogh to clean dirt (go to dst, clean 1 and then get back to docking)
        int stepsToDstAndGetBack = distanceFromDirt + 1 + getDistanceFromDock(dirtyDst);
        if (stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)) {
            // std::cout << thread<<" stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)" << std::endl;
            // std::cout << thread<<" Prefere dirt!" << std::endl;
            shortestPathToDstWithMaximumUnknown(currentLocation, dirtyDst);
            onWayToClean = true;
            // std::cout <<thread<< "onWayToClean = true;" << std::endl;
            // std::cout <<thread<< "getStepFromMapping" << std::endl;
            return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
        }
    }
    if (distanceFromUnkwon != -1) {
        // std::cout << thread<<" distanceFromUnkwon != -1" << std::endl;
        // std::cout << thread <<" got unknown" << std::endl;
        // No dirt can be clean/ found- prefere unkwon
        int stepsToDstAndGetBack = distanceFromUnkwon + getDistanceFromDock(unkwonDst);
        if (stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)) {
            // std::cout << thread<<" stepsToDstAndGetBack <= std::min(batterySteps, maxSteps)" << std::endl;
            shortestPathToDstWithMaximumUnknown(currentLocation, unkwonDst);
            onWayToClean = true;
            return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
        }
    }
    
    // No dirt/ unkwon is found- return to docking and finish
    // std::cout << thread <<" no tile to explore" << std::endl;
    shortestPathToDstWithMaximumUnknown(currentLocation, dockingStationLocation);
    needToReturn = true;
    needToFinish = true;
    // std::cout << thread<<" getStepFromMapping" << std::endl;
    return getStepFromMapping(batterySteps, maxBatterySteps, maxSteps);
}*/

Step HouseMappingGraph::getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) + "]: ";
    // std::cout << thread << "getStepFromMapping currentLocation: " << currentLocation.first << ", " << currentLocation.second << std::endl;

    // If Im on way to clean- get from robotDeterminedPath the next step
    if (onWayToClean || needToReturn) {
        // std::cout <<thread<< "if (onWayToClean)" << std::endl;
        if (!robotDeterminedPath.empty()) {
            // std::cout <<thread<< "!robotDeterminedPath.empty()" << std::endl;
            Step s = getStepFromSrcToDst(currentLocation, robotDeterminedPath.front());
            updateCurrentLocation(s);
            robotDeterminedPath.pop();
            return s;
        }
        // else- done robotDeterminedPath
        // std::cout <<thread<< "onWayToClean = false;" << std::endl;
        onWayToClean = false;
        needToReturn = false;
    }

    if (shouldFinish()) {
        // std::cout <<thread<< "shouldFinish()- return finish" << std::endl;
        return Step::Finish;
    }

    // Handle charging and return to docking station
    if (shouldStayCharging(batterySteps, maxBatterySteps, maxSteps)) {
        // std::cout <<thread<< "shouldStayCharging(batterySteps, maxBatterySteps, maxSteps)- return stay" << std::endl;
        return Step::Stay; // or the appropriate Step based on the condition
    }

    // Check if we need to clean the current location
    if (shouldCleanCurrentLocation(batterySteps, maxSteps)) {
        // std::cout <<thread<< "shouldCleanCurrentLocation(batterySteps, maxSteps)- return stay" << std::endl;
        updateCurrentLocation(Step::Stay);
        return Step::Stay;
    }

    // std::cout <<thread<< "decideNextStep(batterySteps, maxSteps);" << std::endl;
    // Handle cleaning, exploring, or finishing
    return decideNextStep(batterySteps, maxSteps);
}

/*
    Check if need to finish
*/
bool HouseMappingGraph::shouldFinish() {
    // if we are in docking and we need to finish OR we dont have any accessible tile to reach (dirt or unkwon- finish)
    // TODO: add the longer
    //return isDockingStation(currentLocation) && (needToFinish || (dirtyTiles.size() == 0 && unkownTiles.size() == 0));
    return isDockingStation(currentLocation) && needToFinish;
}

/*
    Check if need to stay in docking and charge
*/
bool HouseMappingGraph::shouldStayCharging(int batterySteps, int maxBatterySteps, int maxSteps) {
    // std::cout << "isDockingStation(currentLocation) " << isDockingStation(currentLocation) <<std::endl;
    // std::cout << "needToCharge " << needToCharge<< std::endl;
    // std::cout << "batterySteps " << batterySteps<< std::endl;
    // std::cout << "maxBatterySteps " << maxBatterySteps<< std::endl;
    // std::cout << "maxSteps " << maxSteps<< std::endl;
    if (batterySteps == maxBatterySteps) {
        needToCharge = false;
    }
    return isDockingStation(currentLocation) && needToCharge && batterySteps < maxBatterySteps && maxSteps > 0;
}

bool HouseMappingGraph::shouldCleanCurrentLocation(int batterySteps, int maxSteps) {
    if (tiles[currentLocation] > 0 && canCleanCurrentLocation(batterySteps, maxSteps)) {
        // std::cout << "Cleaning current location." << std::endl;
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

Step HouseMappingGraph::decideNextStep(int batterySteps, int maxSteps) {
    // update distances and targets
    int distanceFromDocking = 0;
    getDistanceFromDockingAndPotentialDst(distanceFromDocking);

    // std::cout << "DirtyDst: " << dirtyDst.first << ", " << dirtyDst.second << " Distance: " << distanceFromDirt << std::endl;
    // std::cout << "UnknownDst: " << unkwonDst.first << ", " << unkwonDst.second << " Distance: " << distanceFromUnkwon << std::endl;

    int distanceOfDirtFromDock = distanceFromDirt == -1 ? -1 : getDistanceFromDock(dirtyDst);
    // Prefer dirt, but if none available, prefer exploring unknown areas
    if (distanceFromDirt != -1 && canReachAndCleanDirt(batterySteps, maxSteps, distanceOfDirtFromDock)) {
        // std::cout << "distanceFromDirt != -1 && canReachAndCleanDirt(batterySteps, maxSteps)" << std::endl;
        onWayToClean = true;
        return getStepToTarget(dirtyDst);
    }

    int distanceOfUnkownFromDock = distanceFromUnkwon == -1 ? -1 : getDistanceFromDock(unkwonDst);
    if (distanceFromUnkwon != -1 && canExploreUnknown(batterySteps, maxSteps, distanceOfUnkownFromDock)) {
        // std::cout << "distanceFromUnkwon != -1 && canExploreUnknown(batterySteps, maxSteps)" << std::endl;
        onWayToClean = true;
        return getStepToTarget(unkwonDst);
    }
    
    shouldNeedToFinish(maxSteps, distanceFromUnkwon, distanceFromDirt, distanceOfUnkownFromDock, distanceOfDirtFromDock);
    // No dirt or unknown areas left, return to docking station
    needToReturn = true;
    needToCharge = true;
    return getStepToTarget(dockingStationLocation);
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

/*
    insert unvisited/ non-wall neighbors to queue
*/
void HouseMappingGraph::updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, 
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
        // Found both potential dst
        if (distanceFromUnkwon != -1 && distanceFromDirt != -1) {
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
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

   if (robotDeterminedPath.size() > 0) {
        throw std::runtime_error(thread + "!!!robotDeterminedPath.size() > 0");
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
        // std::cout <<thread<< " (" << p.first << ", " << p.second << ") ";
        robotDeterminedPath.push(p);
    }
}

void HouseMappingGraph::getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

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
    printStack(fillStack);
    // std::cout << thread<<" getStepsFromParent" << std::endl;

}

void HouseMappingGraph::printStack(std::stack<Step> s) {
    std::string thread = " in thread [" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id())) +"]: ";

    // std::cout << thread<<" printStack" << std::endl;
    std::stack<Step> tempStack;
    while (!s.empty()) {
        // Get the top element
        Step top = s.top();
        // Print it
        switch (top)
        {
        case Step::South:
            // std::cout << "South" << " ";
            break;
        case Step::West:
            // std::cout << "West" << " ";
            break;
        case Step::North:
            // std::cout << "North" << " ";
            break;
        case Step::East:
            // std::cout << "East" << " ";
            break;
        case Step::Stay:
            // std::cout << "Stay" << " ";
            break;
        case Step::Finish:
            // std::cout << "Finish" << " ";
            break;
        }
        // Pop the top element
        s.pop();
        // Push it onto the temporary stack
        tempStack.push(top);
    }
    // std::cout << std::endl;

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
        // std::cout <<"Step::South" << std::endl;
        return Step::South;
    }
    if (diffrenceRow == 1 && diffrenceCol == 0) {
        // std::cout <<"Step::North" << std::endl;
        return Step::North;
    }
    if (diffrenceRow == 0 && diffrenceCol == -1) {
        // std::cout <<"Step::East" << std::endl;
        return Step::East;
    }
    if (diffrenceRow == 0 && diffrenceCol == 1) {
        // std::cout <<"Step::West" << std::endl;
        return Step::West;
    }
    // diffrenceRow == 0 && diffrenceCol == 0) 
    // std::cout <<"Step::Stay" << std::endl;
    return Step::Stay;
}

bool HouseMappingGraph::isVisitedInCurrentLocation() const {
    if (visitedTiles.find(currentLocation) == visitedTiles.end()) {
        return false;
    }
    return true;
}
