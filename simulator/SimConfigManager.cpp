#include "include/sim_config_manager.h"

SimConfigurationManager::SimConfigurationManager() {
    // Set default
    timePerStep = 100;
    try {
        std::string jsonString = readConfig("config/config.json");

        // If the file could not be read, use default parameters.
        if (jsonString == "")
            return;

        loadParametersFromConfigFile(jsonString);
    } catch (const std::exception& e) {
        std::cerr << "Error initializing SimConfigurationManager: " << e.what() << std::endl;
    }
}

void SimConfigurationManager::loadParametersFromConfigFile(std::string& jsonString){
    // Parse the JSON data 
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Check for parse errors 
    if (doc.HasParseError()) {
        std::cerr << "Error initializing SimConfigurationManager: " << doc.GetParseError() << std::endl;
        return;
    }
    
    if (doc.HasMember("timePerStep") && doc["timePerStep"].IsInt()) {
        timePerStep = doc["timePerStep"].GetInt();
    }
}

int SimConfigurationManager::getTimePerStep() const {
    return timePerStep;
}
