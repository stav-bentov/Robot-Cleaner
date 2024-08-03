#ifndef HOUSE_MAPPING_GRAPH_H_
#define HOUSE_MAPPING_GRAPH_H_

#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "house.h"
#include "common.h"
#include "enums.h"
#include "vertex.h"
#include "robot_walls_sensor.h"

class HouseMappingGraph {
    private:
        // Mapping of corrdinate and vetices 
        // unique_ptr because this is the only pointer
        std::map<std::pair<int, int>, std::unique_ptr<Vertex>> verticesMapping;
        // mapping of corrdinate to neigbors vertices
        std::map<std::pair<int, int>, std::set<std::pair<int, int>>> adjacencyList;
        std::pair<int, int>& currentLocation;
        const std::pair<int, int> dockingStationLocation;
        bool startFinish;

        Step getStepByDiff(int diffrenceRow, int diffrenceCol);
        bool isDockingStation(std::pair<int, int> location) const;
        std::pair<int, int> getRelativeLocation(Direction d);
        bool needToReturn(int distanceFromStation, int maxSteps);
        Step getFirstStep(const std::pair<int, int>& target, const std::pair<int, int>& start, const std::map<std::pair<int, int>, std::pair<int, int>>& parent);
        Step getNextStep(bool choseDirtOrUnknownDst, int dirtDistance, std::pair<int, int> dirtOrUnknownDst, 
                                    int maxSteps, int batterySteps, std::map<std::pair<int, int>, std::pair<int, int>>& parent, int maxBatterySteps);
        Step stepToDirection(Direction d);
        void updateCurrentLocation(Step s);
        int getDistanceFromDocking(std::pair<int, int> src);
        bool getStartFinish() const;
        int calculateStayingInDocking(int maxSteps, int maxBatterySteps);
    public:
        HouseMappingGraph();
        void connect(Direction d);
        void addVertex(std::pair<int, int> location, Type t);
        void addVertex(Direction d, Type t);
        Step runBfs(int batterySteps, int maxBatterySteps, int maxSteps);
        void setDirt(std::pair<int, int> location, int dirt);
        bool shouldFinish() const;


};
#endif  // HOUSE_MAPPING_GRAPH_H_