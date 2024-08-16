#ifndef HOUSE_MAPPING_GRAPH_C_H_
#define HOUSE_MAPPING_GRAPH_C_H_

#include <set>
#include <unordered_map>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "../../common_algorithm/include/house.h"
#include "../../common_algorithm/include/common_enums.h"
#include "../../common_algorithm/include/vertex.h"
#include "../../common_algorithm/include/robot_walls_sensor.h"

class HouseMappingGraphC {
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
        HouseMappingGraphC(std::pair<int, int>& location);
        void addVertex(std::pair<int, int> location, Type t);
        void addVertex(Direction d, Type t);
        Step runBfs(int batterySteps, int maxBatterySteps, int maxSteps);
        void setDirt(std::pair<int, int> location, int dirt);
        bool shouldFinish() const;


};
#endif  // HOUSE_MAPPING_GRAPH_C_H_