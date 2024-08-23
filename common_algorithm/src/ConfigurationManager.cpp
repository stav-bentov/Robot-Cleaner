#include "../include/configuration_manager.h"

ConfigurationManager::ConfigurationManager() {}

std::string ConfigurationManager::readConfig(const std::string& configFileName) const{
    // Got help from: https://www.geeksforgeeks.org/rapidjson-file-read-write-in-cpp/
    std::ifstream configFile(configFileName);
    if (!configFile.is_open()) {
        Logger::getInstance().logToConsoleError("Failed to open file 'visualisation_config.json'. Will use default parameters");
        return "";
    }

    // istreambuf_iterator -read the entire file into a string
    std::string jsonString((std::istreambuf_iterator<char>(configFile)),
                           std::istreambuf_iterator<char>());

    configFile.close();
    return jsonString;
}

/* 
    Prevent represetation of robot/ docking station/ wall as digit (this belongs to flooe dirt level)
*/
bool ConfigurationManager::isNotDigit(const std::string& str) {
    if(str.empty() || str.size() > 1)
        return true;
    char ch = str[0];
    return !std::isdigit(ch);
}