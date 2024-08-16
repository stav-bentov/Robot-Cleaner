#ifndef HOUSE_MAPPING_GRAPH_B_H_
#define HOUSE_MAPPING_GRAPH_B_H_

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "../../common_algorithm/include/common_enums.h"

class HouseMappingGraphB {
    private:
        enum class TilesType {
            UnknownDirt = -1,
            DockingStation = -2
        };

        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator ()(const std::pair<T1, T2>& p) const {
                auto h1 = std::hash<T1>{}(p.first);
                auto h2 = std::hash<T2>{}(p.second);
                return h1 ^ h2; // Combine the hash values
            }
        };

        std::unordered_map<std::pair<int, int>, int, pair_hash> tiles;
        std::stack<Step> cleanStack;
        std::stack<Step> toDockingStack;

        std::queue<std::pair<int, int>> robotDeterminedPath;

        const std::pair<int, int> dockingStationLocation;
        std::pair<int, int> currentLocation;
        bool needToReturn;
        bool onWayToClean;
        bool needToFinish;
        bool needToCharge;
        int currentDistanceFromDocking;

        
        int distanceFromDirt;
        int distanceFromUnkwon;
        std::pair<int, int> dirtyDst;
        std::pair<int, int> unkwonDst;
        std::pair<int, int> getRelativeLocation(Direction d);
        bool isDockingStation(std::pair<int, int> location) const;
        void updateCurrentLocation(Step s);
        void setDirt(std::pair<int, int> location, int dirt);
        void reduceDirt(Step s);
        Step directionToStep(Direction d);
        void getDistanceFromDockingAndPotentialDst(int& distanceFromDocking);
        void updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, 
                                std::queue<std::pair<int, int>>& q);
        Step getStepByDiff(int diffrenceRow, int diffrenceCol);
        void getStepsFromParent(std::pair<int, int> dst, std::unordered_map<std::pair<int, int>, std::pair<int, int>, pair_hash>& parent, std::stack<Step>& fillStack);
        int getDistanceFromDock(std::pair<int, int>& dst);
        void printStack(std::stack<Step> s);
        void shortestPathToDstWithMaximumUnknown(std::pair<int, int> start, std::pair<int, int> dst);
        Step getStepFromSrcToDst(std::pair<int, int> src, std::pair<int, int> dst);
    public:
        HouseMappingGraphB();
        void addTile(std::pair<int, int> location, Type t);
        void addTile(Direction d, Type t);
        void setDirt(int dirt);
        Step getStepFromMapping(int batterySteps, int maxBatterySteps, int maxSteps);


};
#endif  // HOUSE_MAPPING_GRAPH_B_H_