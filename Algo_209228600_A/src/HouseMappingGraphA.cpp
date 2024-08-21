#include "../include/house_mapping_graph_A.h"

Step HouseMappingGraphA::decideNextStep(int batterySteps, int maxSteps) {
    // update distances and targets
    int distanceFromDocking = -1;
    getDistanceFromDockingAndPotentialDst(distanceFromDocking);

    std::cout << "DirtyDst: " << dirtyDst.first << ", " << dirtyDst.second << " Distance: " << distanceFromDirt << std::endl;
    std::cout << "UnknownDst: " << unkwonDst.first << ", " << unkwonDst.second << " Distance: " << distanceFromUnkwon << std::endl;

    int distanceOfDirtFromDock = distanceFromDirt == -1 ? -1 : getDistanceFromDock(dirtyDst);
    // Prefer dirt, but if none available, prefer exploring unknown areas
    if (distanceFromDirt != -1 && canReachAndCleanDirt(batterySteps, maxSteps, distanceOfDirtFromDock)) {
        std::cout << "distanceFromDirt != -1 && canReachAndCleanDirt(batterySteps, maxSteps)" << std::endl;
        onWayToClean = true;
        return getStepToTarget(dirtyDst);
    }

    int distanceOfUnkownFromDock = distanceFromUnkwon == -1 ? -1 : getDistanceFromDock(unkwonDst);
    if (distanceFromUnkwon != -1 && canExploreUnknown(batterySteps, maxSteps, distanceOfUnkownFromDock)) {
        std::cout << "distanceFromUnkwon != -1 && canExploreUnknown(batterySteps, maxSteps)" << std::endl;
        onWayToClean = true;
        return getStepToTarget(unkwonDst);
    }
    
    shouldNeedToFinish(maxSteps, distanceFromUnkwon, distanceFromDirt, distanceOfUnkownFromDock, distanceOfDirtFromDock);
    // No dirt or unknown areas left, return to docking station
    needToReturn = true;
    needToCharge = true;
    return getStepToTarget(dockingStationLocation);
}

/*
    insert unvisited/ non-wall neighbors to queue
*/
void HouseMappingGraphA::updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, 
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