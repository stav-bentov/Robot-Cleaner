#ifndef HOUSE_MAPPING_GRAPH_H_
#define HOUSE_MAPPING_GRAPH_H_

#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/common_enums.h"
#include "vertex.h"

class HouseMappingGraph {
    private:
    
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator ()(const std::pair<T1, T2>& p) const {
                auto h1 = std::hash<T1>{}(p.first);
                auto h2 = std::hash<T2>{}(p.second);
                return h1 ^ h2; // Combine the hash values
            }
        };
        // Mapping of corrdinate and vetices 
        std::unordered_map<std::pair<int, int>, std::unique_ptr<Vertex>, pair_hash> verticesMapping;
        const std::pair<int, int> dockingStationLocation;
        std::pair<int, int> currentLocation;
        bool startFinish;

        Step getStepByDiff(int diffrenceRow, int diffrenceCol);
        bool isDockingStation(std::pair<int, int> location) const;
        std::pair<int, int> getRelativeLocation(Direction d);
        bool needToReturn(int distanceFromStation, int maxSteps);
        Step getFirstStep(const std::pair<int, int>& target, const std::pair<int, int>& start, const std::map<std::pair<int, int>, std::pair<int, int>>& parent);
        Step getNextStep(bool choseDirtOrUnknownDst, int dirtDistance, std::pair<int, int> dirtOrUnknownDst, 
                                    int maxSteps, int batterySteps, std::map<std::pair<int, int>, std::pair<int, int>>& parent, int maxBatterySteps);
        void updateCurrentLocation(Step s);
        int getDistanceFromDocking(std::pair<int, int> src);
        bool getStartFinish() const;
        int calculateStayingInDocking(int maxSteps, int maxBatterySteps);
        bool checkLocation(std::pair<int, int>& location, int depth, int batterySteps, int maxSteps, 
                                      std::map<std::pair<int, int>, std::pair<int, int>>& parent, bool& choseDirtOrUnknownDst,
                                      bool isDirtOrUnknown, int& dirtDistance, std::pair<int, int>& dirtOrUnknownDst, Step& s);
        void updateQ(std::pair<int, int>& location, std::set<std::pair<int, int>>& visited, 
                                std::queue<std::pair<int, int>>& q, std::map<std::pair<int, int>, std::pair<int, int>>& parent);
    public:
        HouseMappingGraph();
        void addVertex(std::pair<int, int> location, Type t);
        void addVertex(Direction d, Type t);
        Step runBfs(int batterySteps, int maxBatterySteps, int maxSteps);
        void setDirt(std::pair<int, int> location, int dirt);
        void setDirt(int dirt);
        bool shouldFinish() const;


};
#endif  // HOUSE_MAPPING_GRAPH_H_