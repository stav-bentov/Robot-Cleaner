#include "../include/house_mapping_graph_B.h"

Step HouseMappingGraphB::decideNextStep(int batterySteps, int maxSteps) {
    getPotentialDst(false);
    std::cout << "HouseMappingGraphB::decideNextStep: distanceInGeneral =  " << distanceInGeneral <<std::endl;
    std::cout << "HouseMappingGraphB::decideNextStep: generalDst =  " << generalDst.first << ", "<<generalDst.second <<std::endl;
    if (distanceInGeneral != -1 && enoghBatteryAndMaxSteps(distanceInGeneral, getDistanceFromDock(generalDst), batterySteps, maxSteps)) {
        std::cout << "getStepToTarget(generalDst)" <<std::endl;
        onWayToClean = true;
        return getStepToTarget(generalDst);
    }
    
    // No dirt or unknown areas left, return to docking station
    needToReturn = true;
    needToCharge = true;
    return getStepToTarget(dockingStationLocation);
}

// Update queue with random order (so there is not priorty for certin direction in every step)
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