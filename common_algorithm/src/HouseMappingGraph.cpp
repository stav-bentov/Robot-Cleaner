#include "../include/house_mapping_graph.h"

HouseMappingGraph::HouseMappingGraph() : dockingStationLocation(0, 0), 
                                         currentLocation(0, 0), // Initialize with default values
                                         startFinish(false) {
        addVertex(dockingStationLocation, Type::DockingStation);
}

void HouseMappingGraph::addVertex(std::pair<int, int> location, Type t) {
    // If vertex doesnt exist (accordint to its coordinate)- add it
    if (verticesMapping.find(location) == verticesMapping.end()) {  
        Logger::getInstance().log("HouseMappingGraph::addVertex in : " + std::to_string(location.first) + ", " + std::to_string(location.second) + ".\n", 3);
        // Create vertex and add it to the mapping
        vertices.emplace_back(location, t);
        verticesMapping[location] = vertices.size() - 1;
    }
}

void HouseMappingGraph::printVerticesMapping() {
    Logger::getInstance().log("HouseMappingGraph::printVerticesMapping\n", 3);

    for (const auto& vertex : vertices) {
        const auto& location = vertex.getLocation();
        
        // Logging the location
        Logger::getInstance().log("HouseMappingGraph::Location: " + std::to_string(location.first) + ", " + std::to_string(location.second) + "\n", 3);
        // Checking if the vertex pointer is valid and logging its data
        Logger::getInstance().log("HouseMappingGraph::Dirt Level: " + std::to_string(vertex.getDirt()) + "\n", 3);
        Logger::getInstance().log("HouseMappingGraph::Dirt is known: " + std::to_string(vertex.getIsDirtKnown()) + "\n", 3);


        Logger::getInstance().log("\n", 3);  // Logging a new line for better readability
    }

}

void HouseMappingGraph::addVertex(Direction d, Type t) {
    std::pair<int, int> location = getRelativeLocation(d);
    // If vertex doesnt exist (accordint to its coordinate)- add it
    addVertex(location, t);
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
    // Docking station or a wall (no dirt)
    if (dirt < 0) {
        Logger::getInstance().log("HouseMappingGraph:: dirt < 0 in : " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", 3);
        return;
    }

    // Update dirt (assumption location IS exist)
    Vertex* v = &vertices[verticesMapping[location]];
    v->setDirt(dirt);
    v->updateType(Type::Floor);
    // if I set the dirt then I visited in it!
    v->setVisited();
}

void HouseMappingGraph::setDirt(int dirt) {
    setDirt(currentLocation, dirt);
}

bool HouseMappingGraph::needToReturn(int distanceFromStation, int maxSteps) {
    Logger::getInstance().log("HouseMappingGraph::needToReturn: maxSteps=" + std::to_string(maxSteps) + ", distanceFromStation" + std::to_string(distanceFromStation) + ".\n", 3);

    // The robot should return when the remaining steps are less than or equal to the distance from the docking station
    return (maxSteps - distanceFromStation == 0);
}

/*
    According to parent mapping that is created by the BFS run- get the first step that leads start to target
*/
Step HouseMappingGraph::getFirstStep(const std::pair<int, int>& target, const std::pair<int, int>& start, const std::map<std::pair<int, int>, std::pair<int, int>>& parent) {
    std::pair<int, int> current = target;
    std::pair<int, int> prev = target;

    while (current != start) {
        prev = current;
        current = parent.at(current);
    }

    int rowDiff = start.first - prev.first;
    int colDiff = start.second - prev.second;
    return getStepByDiff(rowDiff, colDiff);
}

void HouseMappingGraph::updateCurrentLocation(Step s) {
    Logger::getInstance().log("HouseMappingGraph:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", 3);

    // Assumption- vertex currentLocation exist
    Vertex* vertex = &vertices[verticesMapping[currentLocation]];
    std::pair<int, int> stepElements = House::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
    if (s == Step::Stay) {
        if (vertex->getDirt() > 0) { 
            // Clean here (not docking station)
            vertex->reduceDirt();
        }
    }
    Logger::getInstance().log("HouseMappingGraph:: Update location from: " + std::to_string(currentLocation.first) + ", " + std::to_string(currentLocation.second) + ".\n", 3);

}

/*
    Run bfs and calculate next step
*/
Step HouseMappingGraph::runBfs(int batterySteps, int maxBatterySteps, int maxSteps) {
    // Run Bfs to find next step
    //Logger::getInstance().getLogger()->info("Start bfs in house mapping graph");
    int depth = 0;
    int dirtDistance = 0;
    bool choseDirtOrUnknownDst = false;
    std::pair<int, int> dirtOrUnknownDst;
    Step s;
    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visited;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    // Set queue
    q.push(currentLocation);
    parent[currentLocation] = currentLocation;
    visited.insert(currentLocation);

    // Bfs while loop (will stop if found out that need to return to docking station that it has a step or finish)
    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();
            
            Vertex* vertex = &vertices[verticesMapping[location]];
            bool isDirtOrUnknown = (vertex->getDirt() > 0 || !vertex->getIsDirtKnown());
            
            if (checkLocation(location, depth, batterySteps, maxSteps, parent,choseDirtOrUnknownDst,
                          isDirtOrUnknown, dirtDistance, dirtOrUnknownDst, s))
            {
                Logger::getInstance().log("HouseMappingGraph::runBfs Returned step from checkLocation.\n", 3);
                return s;
            }

            updateQ(location, visited, q, parent);
        }
        Logger::getInstance().log("HouseMappingGraph::runBfs Done while loop for depth: " + std::to_string(depth) +".\n", 3);

        ++depth;

        // No need to continue in depth more then maxBatterySteps or maxSteps (not enogh steps)
        if (depth > std::min(batterySteps, maxSteps)) {
            break;
        }
    }

    // No reachable dirt/ unknown location and location is in docking station- return Finish! 
    if ((!choseDirtOrUnknownDst && isDockingStation(currentLocation))) {
        Logger::getInstance().log("HouseMappingGraph::runBfs Returned finish from (!choseDirtOrUnknownDst && isDockingStation(currentLocation).\n", 3);
        return Step::Finish;
    }

    // Charge only if batterySteps < maxBatterySteps
    // Else- go to closest dirt
    if (isDockingStation(currentLocation) && batterySteps < maxBatterySteps) {
        Logger::getInstance().log("HouseMappingGraph::runBfs Returned step from isDockingStation(currentLocation) && batterySteps < maxBatterySteps.\n", 3);

        // In docking station and cant clean with 1 step left
        if (maxSteps == 1) {
            return Step::Finish;
        }
        return Step::Stay;
    }

    // Else- get in direction of next dst
    Logger::getInstance().log("HouseMappingGraph::runBfs Returned Else- get in direction of next dst.\n", 3);

    s = getNextStep(choseDirtOrUnknownDst, dirtDistance, dirtOrUnknownDst, maxSteps, batterySteps, parent, maxBatterySteps);
    updateCurrentLocation(s);
    return s;
}

/*
    insert unvisited/ non-wall neighbors to queue
*/
void HouseMappingGraph::updateQ(std::pair<int, int>& location, std::set<std::pair<int, int>>& visited, 
                                std::queue<std::pair<int, int>>& q, std::map<std::pair<int, int>, std::pair<int, int>>& parent) {
    for (const auto& entry : Common::directionMap) {  
        std::pair<int, int> nei = location;
        nei.first += entry.second.first;
        nei.second += entry.second.second;

        Vertex* neiVertex = &vertices[verticesMapping[nei]];
        std::pair<int, int> neiLocation = neiVertex->getLocation();
        // Wall- no way to continue
        if (neiVertex->isWall()) {
            continue;
        }

        // Not a wall- check if not visited, if not add to queue
        if (visited.find(neiLocation) == visited.end()) {
            visited.insert(neiLocation);
            q.push(neiLocation);
            parent[neiLocation] = location; // Set parent for the new node
        }
    }
}

/*
    Gets location from BFS while loop.
    Check if it is a docking station and if need to go back there (not enohg steps), or need to finish.
    Check if this location has the closest dirt or unknown dirt- save this location.
    Returns true if a s was changes and we need to make it, else- false.
*/
bool HouseMappingGraph::checkLocation(std::pair<int, int>& location, int depth, int batterySteps, int maxSteps, 
                                      std::map<std::pair<int, int>, std::pair<int, int>>& parent, bool& choseDirtOrUnknownDst,
                                      bool isDirtOrUnknown, int& dirtDistance, std::pair<int, int>& dirtOrUnknownDst, Step& s) {
    bool returnS = false;
    // Check distance from docking station and if we need to get back
    if (isDockingStation(location)) {
        // If robot doesnt have enogh steps- take first step in the direction of docking station
        if (needToReturn(depth, std::min(maxSteps, batterySteps))) {
            // If it needs to return because of maxSteps then start finish
            if (needToReturn(depth, maxSteps)) {
                startFinish = true;
                // If in docking- return finish!
                if (isDockingStation(currentLocation)) {
                    returnS = true;
                    s = Step::Finish;
                }
            }
            
            s = getFirstStep(location, currentLocation, parent);
            updateCurrentLocation(s);
            //Logger::getInstance().getLogger()->info("need to return to docking station because number of steps is: {} and distance is: {}", maxSteps, depth);

            returnS = true;
        }
    } else {
        /* If location has dirt- this is closest dirt then take step to this direction
            OR
            location has unknown dirt- get there
        */
        if (!choseDirtOrUnknownDst && isDirtOrUnknown) {
            Logger::getInstance().log("choseDirtOrUnknownDst: " + std::to_string(choseDirtOrUnknownDst) + ", isDirtOrUnknown" + std::to_string(isDirtOrUnknown) + "\n", 3);

            dirtDistance = depth;
            choseDirtOrUnknownDst = true;
            dirtOrUnknownDst = location;
            Logger::getInstance().log("HouseMappingGraph:: dirt dst: " + std::to_string(location.first) + ", " + std::to_string(location.second) + "\n", 3);
            Logger::getInstance().log("depth: " + std::to_string(depth) +"\n", 3);
        }
    }
    return returnS;
}

int HouseMappingGraph::getDistanceFromDocking(std::pair<int, int> src) {
    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visited;
    int depth = 0;
    std::map<std::pair<int, int>, std::pair<int, int>> parent;

    // Set queue
    q.push(src);
    visited.insert(src);

    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();
            
            // Distance from docking station is depth
            if (isDockingStation(location)) {
                Logger::getInstance().log("HouseMappingGraph::getDistanceFromDocking return : " + std::to_string(depth) +"\n", 3);
                return depth;
            }
            updateQ(location, visited, q, parent);
        }
        ++depth;
    }
    // Wont heppen!
    //Logger::getInstance().getLogger()->error("Error-> Didnt reach docking station!");
    Logger::getInstance().log("HouseMappingGraph::getDistanceFromDocking Error-> Didnt reach docking station!: \n", 3);

    return -1;
}

Step HouseMappingGraph::getNextStep(bool choseDirtOrUnknownDst, int dirtDistance, std::pair<int, int> dirtOrUnknownDst, 
                                    int maxSteps, int batterySteps, std::map<std::pair<int, int>, std::pair<int, int>>& parent, int maxBatterySteps) {
    Step s;
    Logger::getInstance().log("choseDirtOrUnknownDst: " + std::to_string(choseDirtOrUnknownDst) + ", " 
                                "dirtDistance: " + std::to_string(dirtDistance) + ", " + 
                                "maxSteps: " + std::to_string(maxSteps) + ", " + 
                                "batterySteps: " + std::to_string(batterySteps) + ", " + 
                                "maxBatterySteps: " + std::to_string(maxBatterySteps) + ", " + 
                                "dirtOrUnknownDst: " + std::to_string(dirtOrUnknownDst.first) + ", " + std::to_string(dirtOrUnknownDst.second) + "\n", 3);

    // Calculate next step
    // Chose to go to dirt/ unknown dirt location
    if (choseDirtOrUnknownDst) {
        int stepsToCleanAndGetBack = dirtDistance + 1 + getDistanceFromDocking(dirtOrUnknownDst);
        Logger::getInstance().log("stepsToCleanAndGetBack: " + std::to_string(stepsToCleanAndGetBack) + "\n", 3);
         /* When do I need to start finish? 
            if I wanted to clean the closest dirt I have, there wasnt enogh steps left.
            or the maxSteps is enogh to clean this dirt, but- the battery steps doesnt, then need to get back to docking
        */
        if (stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > batterySteps) {
            Logger::getInstance().log("stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > batterySteps TRUE: \n", 3);
            s = getFirstStep(dockingStationLocation, currentLocation, parent);
            // Cant clean this dirt/ unknown tile because there are not enogh steps
            if (stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > maxBatterySteps) {
                Logger::getInstance().log("stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > maxBatterySteps TRUE:\n", 3);

                if (isDockingStation(currentLocation)) {
                    s = Step::Finish;
                }
                else {
                    startFinish = true;
                    Logger::getInstance().log("Go to back to docking and start finish\n", 3);

                    //Logger::getInstance().getLogger()->info("Go to back to docking and start finish");
                }
            }
        }
        // Else- maxSteps and batterySteps is enogh for this cleaning- go there
        else {
            s = getFirstStep(dirtOrUnknownDst, currentLocation, parent);
            Logger::getInstance().log("Go to clean a tile\n", 3);
            //Logger::getInstance().getLogger()->info("Go to clean a tile");
        }
    } 
    // No unknown location is reachable
    // No need to get back to docking station
    // No dirt to clean
    // What step should we do? get back to docking and start finish
    else { // All tiles are visited
        Logger::getInstance().log("All tiles are visited\n", 3);
        s = getFirstStep(dockingStationLocation, currentLocation, parent);
        startFinish = true;
    }
    return s;
}

Step HouseMappingGraph::stepToDirection(Direction d) {
    Step s;
    switch (d)
    {
        case Direction::East:
            s = Step::East;
            break;
        case Direction::West:
            s = Step::West;
            break;
        case Direction::North:
            s = Step::North;
            break;
        case Direction::South:
            s = Step::South;
            break;
    }
    return s;
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

bool HouseMappingGraph::getStartFinish() const{
    return startFinish;
}

bool HouseMappingGraph::shouldFinish() const{
    if (getStartFinish() && isDockingStation(currentLocation)) {
        return true;
    }
    return false;
}