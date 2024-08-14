#ifndef CSV_MANAGER_H
#define CSV_MANAGER_H

#include "../../common_algorithm/include/common_enums.h"

class CsvManager {
    private:
        std::vector<std::string> algorithmNames;
        std::vector<std::string> housesNames;
        // rows are the algorithm, cols are the houses
        std::vector<std::vector<int>> scores;
    public:
        void addAlgorithmName(std::string algoName);
        void addHouseName(std::string houseName);
        void addScore(int score, int algoIdx, int houseIdx);
        void printToCsv();
};

#endif  // CSV_MANAGER_H