#include "include/csv_manager.h"


void CsvManager::writeResultsToCsv() {
    std::ofstream file("summary.csv");
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing.");
    }

    file << "Algorithm\\House";
    for (const auto& house : housesNames) {
        file << "," << house;
    }
    file << "\n";

    for (std::size_t algoIdx = 0; algoIdx < algorithmNames.size(); algoIdx++) {
        file << algorithmNames[algoIdx];
        for (std::size_t houseIdx = 0; houseIdx < housesNames.size(); houseIdx++) {
            if (scores[algoIdx][houseIdx] != -1) {
                file << "," << scores[algoIdx][houseIdx];
            }
            else {
                file << "," << "NA";
            }
        }
        file << "\n";
    }

    file.close();
}