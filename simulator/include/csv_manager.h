#ifndef CSV_MANAGER_H
#define CSV_MANAGER_H

#include "../../common_algorithm/include/common_enums.h"
#include <algorithm>
class CsvManager {
    private:
        std::vector<std::string> algorithmNames;
        std::vector<std::string> housesNames;
        std::vector<std::vector<int>>& scores;
    public:
        CsvManager(std::vector<std::string>& algorithmNames, std::vector<std::string>& housesNames, std::vector<std::vector<int>>& scores) 
                                                            : algorithmNames(algorithmNames), housesNames(housesNames), scores(scores){};
        void writeResultsToCsv();
};

#endif  // CSV_MANAGER_H