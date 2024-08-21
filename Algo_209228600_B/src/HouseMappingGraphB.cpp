#include "../include/house_mapping_graph_B.h"

Step HouseMappingGraphB::decideNextStep(int batterySteps, int maxSteps) {
    std::queue<std::pair<int, int>> q;
    std::unordered_set<std::pair<int, int>, pair_hash> visited;
    
    int distanceFromDocking = -1;
    int depth = 0;
    std::pair<int, int> dst;
    int tileData;
    int distance = -1;

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
            else if (tiles[location] != 0) {
                dst = location;
                tileData = tiles[location];
                distance = depth;
            }
            updateQ(location, visited, q);
        }
        if (distanceFromDocking != -1 && distance != -1) {
            break;
        }
        ++depth;
    }
    
    if (distance != -1) {
        int distanceFromDockAndDst = getDistanceFromDock(dst);
        // check if can get to it
        if ((tileData == -1 && distance + distanceFromDockAndDst <= std::min(batterySteps, maxSteps))
            ||  (tileData > 0 && distance + 1 + distanceFromDockAndDst <= std::min(batterySteps, maxSteps))){
            onWayToClean = true;
            return getStepToTarget(dst);
        }
        
        // Not enogh steps check if need to return AND finish
        if ((tileData == -1 && distance + distanceFromDockAndDst > maxSteps)
            ||  (tileData > 0 && distance + 1 + distanceFromDockAndDst > maxSteps)){
            needToFinish = true;
        }
    }
    
    // No dirt or unknown areas left, return to docking station
    needToReturn = true;
    needToCharge = true;
    return getStepToTarget(dockingStationLocation);
}

void HouseMappingGraphB::updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, std::queue<std::pair<int, int>>& q) {
    std::vector<Direction> directions = {
        Direction::North, Direction::East, Direction::South, Direction::West
    };
    auto rd = std::random_device {}; 
    auto rng = std::default_random_engine { rd() };
    std::shuffle(std::begin(directions), std::end(directions), rng);

    for (Direction d : directions) {
        auto entry = Common::directionMap.at(d);  // Use shuffled index to access direction map
        std::pair<int, int> neiLocation = {location.first + entry.first, location.second + entry.second};

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