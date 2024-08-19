#include "include/sim_config_manager.h"

SimConfigurationManager::SimConfigurationManager() {
    // Set default
    maxSteps = 500;
    // TODO: add try- cath and write"set as default"
    std::string jsonString = readConfig("config/config.json");
    // Default parameters...
    if (jsonString == "")
        return;
    loadParametersFromConfigFile(jsonString);
}

void SimConfigurationManager::loadParametersFromConfigFile(std::string& jsonString){
    // Parse the JSON data 
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Check for parse errors 
    if (doc.HasParseError()) {
        throw std::runtime_error("Parse error:" + doc.GetParseError());
        // TODO: avoid throw!
    }
    
    if (doc.HasMember("maxSteps") && doc["maxSteps"].IsInt()) {
        maxSteps = doc["maxSteps"].GetInt();
    }
}

int SimConfigurationManager::getMaxSteps() const {
    return maxSteps;
}
