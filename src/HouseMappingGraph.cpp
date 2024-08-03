#include "../simulator/include/house_mapping_graph.h"

HouseMappingGraph::HouseMappingGraph() : currentLocation({0, 0}), dockingStationLocation({0, 0}), startFinish(false) {
    addVertex(dockingStationLocation, Type::DockingStation);
}

void HouseMappingGraph::addVertex(std::pair<int, int> location, Type t) {
    // If vertex doesnt exist (accordint to its coordinate)- add it
    if (verticesMapping.find(location) == verticesMapping.end()) {   
        Logger::getInstance().getLogger()->info("Created vertex in location: " + std::to_string(location.first) + ", " + std::to_string(location.second)); 
        // Create vertex and add it to the mapping
        auto newVertex = std::make_unique<Vertex>(location, t);
        verticesMapping[location] = std::move(newVertex);
    }
}

void HouseMappingGraph::addVertex(Direction d, Type t) {
    std::pair<int, int> location = getRelativeLocation(d);
    // If vertex doesnt exist (accordint to its coordinate)- add it
    addVertex(location, t);
}

std::pair<int, int> HouseMappingGraph::getRelativeLocation(Direction d) {
    std::pair<int, int> cord = House::directionMap.at(d);
    std::pair<int, int> location = std::make_pair(currentLocation.first + cord.first, currentLocation.second + cord.second);
    return location;
}

bool HouseMappingGraph::isDockingStation(std::pair<int, int> location) const {
    return location == dockingStationLocation;
}

void HouseMappingGraph::setDirt(std::pair<int, int> location, int dirt) {
    // Docking station or a wall (no dirt)
    if (dirt < 0) {
        return;
    }

    // Update dirt (assumption location IS exist)
    std::unique_ptr<Vertex>& v = verticesMapping.at(location);
    v->setDirt(dirt);
    v->updateType(Type::Floor);
    // if I set the dirt then I visited in it!
    v->setVisited();
}

void HouseMappingGraph::connect(Direction d) {
    std::pair<int, int> dst = getRelativeLocation(d); 
    adjacencyList[currentLocation].insert(dst);
    adjacencyList[dst].insert(currentLocation);
}

bool HouseMappingGraph::needToReturn(int distanceFromStation, int maxSteps) {
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
    std::unique_ptr<Vertex>& vertex = verticesMapping.at(currentLocation);
    std::pair<int, int> stepElements = House::stepMap.at(s);
    currentLocation.first += stepElements.first;
    currentLocation.second += stepElements.second;
    if (s == Step::Stay) {
        if (vertex->getDirt() > 0) { 
            // Clean here (not docking station)
            vertex->reduceDirt();
        }
    }
}

/*
    Run bfs and calculate next step
*/
Step HouseMappingGraph::runBfs(int batterySteps, int maxBatterySteps, int maxSteps) {
    // Run Bfs to find next step
    Logger::getInstance().getLogger()->info("Start bfs in house mapping graph");
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

    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();
            std::unique_ptr<Vertex>& vertex = verticesMapping.at(location);
            
            // Check distance from docking station and if we need to get back
            if (isDockingStation(location)) {
                // If robot doesnt have enogh steps- take first step in the direction of docking station
                if (needToReturn(depth, std::min(maxSteps, batterySteps))) {
                    // If it needs to return because of maxSteps then start finish
                    if (needToReturn(depth, maxSteps)) {
                        startFinish = true;
                        // If in docking- return finish!
                        if (isDockingStation(currentLocation)) {
                            return Step::Finish;
                        }
                    }
                    
                    s = getFirstStep(location, currentLocation, parent);
                    updateCurrentLocation(s);
                    Logger::getInstance().getLogger()->info("need to return to docking station because number of steps is: {} and distance is: {}", maxSteps, depth);

                    return s;
                }
            } else {
                /* If location has dirt- this is closest dirt then take step to this direction
                    OR
                    location has unknown dirt- get there
                */
                if (!choseDirtOrUnknownDst && (vertex->getDirt() > 0 || !vertex->getIsDirtKnown())) {
                    dirtDistance = depth;
                    choseDirtOrUnknownDst = true;
                    dirtOrUnknownDst = location;
                    Logger::getInstance().getLogger()->info("Find place with dirt: <{}, {}>, {}.", location.first, location.second, vertex->getDirt());
                }
            }

            auto it = adjacencyList.find(location);
            // If current vertex has neighbors- get them into q
            if (it != adjacencyList.end()) {
                for (const auto& nei : it->second) {
                    std::unique_ptr<Vertex>& neiVertex = verticesMapping.at(nei);
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
        }
        ++depth;

        // No need to continue in depth more then maxBatterySteps or maxSteps (not enogh steps)
        if (depth > std::max(batterySteps, maxSteps)) {
            break;
        }
    }

    // No reachable dirt/ unknown location and location is in docking station- return Finish! 
    if ((!choseDirtOrUnknownDst && isDockingStation(currentLocation))) {
        return Step::Finish;
    }
    // Charge inly if batterySteps < maxBatterySteps
    // Else- go to closest dirt
    if (isDockingStation(currentLocation) && batterySteps < maxBatterySteps) {
        // In docking station and cant clean with 1 step left
        if (maxSteps == 1) {
            return Step::Finish;
        }
        return Step::Stay;
    }

    // Else- get in direction of next dst
    s = getNextStep(choseDirtOrUnknownDst, dirtDistance, dirtOrUnknownDst, maxSteps, batterySteps, parent, maxBatterySteps);
    updateCurrentLocation(s);
    return s;
}

int HouseMappingGraph::getDistanceFromDocking(std::pair<int, int> src) {
    std::queue<std::pair<int, int>> q;
    std::set<std::pair<int, int>> visited;
    int depth = 0;

    // Set queue
    q.push(src);

    while(!q.empty()) {
        int size = q.size();

        // Add each neighbor of each tile in queue to it and update the distance from docking station
        for (int i = 0; i < size; ++i) {
            std::pair<int, int> location = q.front();
            q.pop();
            
            // Distance from docking station is depth
            if (isDockingStation(location)) {
                return depth;
            }

            auto it = adjacencyList.find(location);
            // If current vertex has neighbors- get them into q
            if (it != adjacencyList.end()) {
                for (const auto& nei : it->second) {
                    std::unique_ptr<Vertex>& neiVertex = verticesMapping.at(nei);
                    std::pair<int, int> neiLocation = neiVertex->getLocation();
                    // Wall- no way to continue
                    if (neiVertex->isWall()) {
                        continue;
                    }

                    // Not a wall- check if not visited, if not add to queue
                    if (visited.find(neiLocation) == visited.end()) {
                        visited.insert(neiLocation);
                        q.push(neiLocation);
                    }
                }
            }
        }
        ++depth;
    }
    // Wont heppen!
    Logger::getInstance().getLogger()->error("Error-> Didnt reach docking station!");
    return -1;
}

Step HouseMappingGraph::getNextStep(bool choseDirtOrUnknownDst, int dirtDistance, std::pair<int, int> dirtOrUnknownDst, 
                                    int maxSteps, int batterySteps, std::map<std::pair<int, int>, std::pair<int, int>>& parent, int maxBatterySteps) {
    Step s;
    // Calculate next step
    // Chose to go to dirt/ unknown dirt location
    if (choseDirtOrUnknownDst) {
        int stepsToCleanAndGetBack = dirtDistance + 1 + getDistanceFromDocking(dirtOrUnknownDst);
         /* When do I need to start finish? 
            if I wanted to clean the closest dirt I have, there wasnt enogh steps left.
            or the maxSteps is enogh to clean this dirt, but- the battery steps doesnt, then need to get back to docking
        */
        if (stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > batterySteps) {
            s = getFirstStep(dockingStationLocation, currentLocation, parent);
            // Cant clean this dirt/ unknown tile because there are not enogh steps
            if (stepsToCleanAndGetBack > maxSteps || stepsToCleanAndGetBack > maxBatterySteps) {
                if (isDockingStation(currentLocation)) {
                    s = Step::Finish;
                }
                else {
                    startFinish = true;
                    Logger::getInstance().getLogger()->info("Go to back to docking and start finish");
                }
            }
        }
        // Else- maxSteps and batterySteps is enogh for this cleaning- go there
        else {
            s = getFirstStep(dirtOrUnknownDst, currentLocation, parent);
            Logger::getInstance().getLogger()->info("Go to clean a tile");
        }
    } 
    // No unknown location is reachable
    // No need to get back to docking station
    // No dirt to clean
    // What step should we do? get back to docking and start finish
    else { // All tiles are visited
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