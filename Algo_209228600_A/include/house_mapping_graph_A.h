#ifndef HOUSE_MAPPING_GRAPH_A_H_
#define HOUSE_MAPPING_GRAPH_A_H_

#include <set>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include "../../common_algorithm/include/house_mapping_graph.h"

class HouseMappingGraphA : public HouseMappingGraph{
    public:
        Step decideNextStep(int batterySteps, int maxSteps) override;
        void updateQ(std::pair<int, int>& location, std::unordered_set<std::pair<int, int>, pair_hash>& visited, 
                                std::queue<std::pair<int, int>>& q) override;
};
#endif