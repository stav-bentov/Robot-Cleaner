#include "include/csv_manager.h"


void CsvManager::writeResultsToCsv() {
    std::ofstream file("summary.csv");
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing.");
    }

    // Eracse spaces
    std::vector<std::string> modifiedHousesNames;
    for (std::string house : housesNames) {
        std::string result = house;
        result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
        result.erase(std::remove(result.begin(), result.end(), '\r'), result.end());
        modifiedHousesNames.push_back(result);
    }

    // Write header: "Algorithm\House"
    file << "Algorithm\\House";
    for (const auto& house : modifiedHousesNames) {
        file << "," << house;
    }
    file << "\n";

    // Write algorithm names and their corresponding scores
    for (std::size_t algoIdx = 0; algoIdx < algorithmNames.size(); algoIdx++) {
        // Invalid algorithm, skip
        if (algorithmNames[algoIdx][0] == -2) {
            continue;
        }

        // Write the algorithm name
        file << algorithmNames[algoIdx];

        // Write the scores for each house
        for (std::size_t houseIdx = 0; houseIdx < housesNames.size(); houseIdx++) {
            if (scores[algoIdx][houseIdx] != -1) {
                file << "," << scores[algoIdx][houseIdx];
            } else {
                std::cerr << "house- algo with no result!" << std::endl;
                file << ",NA";
            }
        }
        file << "\n";  // Move to the next line after writing the row
    }

    file.close();
}