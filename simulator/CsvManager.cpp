#include "include/csv_manager.h"


void CsvManager::addAlgorithmName(std::string algoName) {
    algorithmNames.push_back(algoName);
}

void CsvManager::addHouseName(std::string houseName) {
    housesNames.push_back(houseName);
}

void CsvManager::addScore(int score, int algoIdx, int houseIdx){ 
    if (algoIdx >= scores.size()) {
        scores.resize(algoIdx + 1); // Resize outer vector if needed
    }
    if (houseIdx >= scores[algoIdx].size()) {
        scores[algoIdx].resize(houseIdx + 1); // Resize inner vector if needed
    }

    // Add the score to the correct location
    scores[algoIdx][houseIdx] = score;
}

void CsvManager::printToCsv() {
    std::ofstream file("summary.csv");
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing.");
    }

    file << ","; 
    for (const auto& houseName : housesNames) {
        file << houseName << ","; 
    }
    file << "\n"; 

    for (size_t algoIdx = 0; algoIdx < algorithmNames.size(); ++algoIdx) {
        file << algorithmNames[algoIdx] << ","; // Write algorithm name

        for (size_t houseIdx = 0; houseIdx < housesNames.size(); ++houseIdx) {
            if (scores[algoIdx][houseIdx] == -1)
                file << "NA" << ",";
            else
                file << scores[algoIdx][houseIdx] << ",";
        }
        file << "\n";
    }

    file.close();
}