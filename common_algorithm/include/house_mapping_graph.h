#ifndef HOUSE_MAPPING_GRAPH_H_
#define HOUSE_MAPPING_GRAPH_H_

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <thread>
#include <cmath>
#include "common_enums.h"


class HouseMappingGraph {
    protected:
        enum class TilesType {
            UnknownDirt = -1,
            DockingStation = -2
        };

        std::unordered_map<std::pair<int, int>, int, pair_hash> tiles;
        std::unordered_set<std::pair<int, int>, pair_hash> visitedTiles;
        std::unordered_set<std::pair<int, int>, pair_hash> dirtyTiles;
        std::unordered_set<std::pair<int, int>, pair_hash> unkownTiles;

        // Contain the shortest path (with most unkwon tiles) to clean a tile
        std::stack<Step> cleanStack;
        std::stack<Step> toDockingStack;

        // Contain the shortest path (with most unkwon tiles) to docking satation/ to clean a tile
        std::queue<std::pair<int, int>> robotDeterminedPath;

        const std::pair<int, int> dockingStationLocation;
        std::pair<int, int> currentLocation;
        bool needToReturn;
        bool onWayToClean;
        bool needToFinish;
        bool needToCharge;
        bool onDeterminedWayFromCharging;
        int currentDistanceFromDocking;
        int stepsToCharge;
        int distanceFromDirt;
        int distanceFromUnkwon;
        int distanceInGeneral;
        std::pair<int, int> dirtyDst;
        std::pair<int, int> unkwonDst;
        std::pair<int, int> generalDst;

        std::pair<int, int> getRelativeLocation(Direction d);
        bool isDockingStation(std::pair<int, int> location) const;
        void updateCurrentLocation(Step s);
        void setDirt(std::pair<int, int> location, int dirt);
        void reduceDirt(Step s);
        Step directionToStep(Direction d);
        void getPotentialDst(bool priority);
        Step getStepByDiff(int diffrenceRow, int diffrenceCol);
        void getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack);
        int getDistanceFromDock(std::pair<int, int>& dst);
        void printStack(std::stack<Step> s);
        void shortestPathToDstWithMaximumUnknown(std::pair<int, int> start, std::pair<int, int> dst);
        Step getStepFromSrcToDst(std::pair<int, int> src, std::pair<int, int> dst);
        bool shouldStayCharging(int batterySteps, int maxBatterySteps, int maxSteps);
        Step getStepToTarget(std::pair<int, int> target);
        bool canExploreUnknown(int batterySteps, int maxSteps, int distanceBetweenDockAndDst);
        bool canReachAndCleanDirt(int batterySteps, int maxSteps, int distanceBetweenDockAndDst);
        bool canCleanCurrentLocation(int batterySteps, int maxSteps);
        bool shouldCleanCurrentLocation(int batterySteps, int maxSteps);
        bool shouldFinish();
        bool shouldNeedToFinish(int batterySteps, int maxBatterySteps, int maxSteps, int distance);
        bool checkCloseDst(Step& s);
        bool enoghBatteryAndMaxSteps(int distanceFromDst, int DistanceBetweenDstAndDock, int batterySteps, int maxSteps);
    public:
        HouseMappingGraph();
        void addTile(std::pair<int, int> location, Type t);
        void addTile(Direction d, Type t);
        void setDirt(int dirt);
        Step getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps);
        bool isVisitedInCurrentLocation() const;
        virtual Step decideNextStep(int batterySteps, int maxSteps) = 0;
        virtual void updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, std::queue<std::pair<int, int>>& q) = 0;


};
#endif  // HOUSE_MAPPING_GRAPH_H_