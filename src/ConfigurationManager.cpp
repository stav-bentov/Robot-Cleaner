#include "../simulator/include/configuration_manager.h"

ConfigurationManager::ConfigurationManager() 
    : displaySimulation(false),
      dockingStationRepresentation("D"),  
      robotRepresentation("R"),            
      wallRepresentation("#") {
    setDefaultParameters();
    std::string jsonString = readConfig();
    // Default parameters...
    if (jsonString == "")
        return;
    loadParametersFromConfigFile(jsonString);
}

std::string ConfigurationManager::readConfig() const{
    // Got help from: https://www.geeksforgeeks.org/rapidjson-file-read-write-in-cpp/
    std::ifstream configFile("config/config.json");
    if (!configFile.is_open()) {
        Logger::getInstance().getLogger()->error("Failed to open file 'config.json'. Will use default parameters.");
        std::cerr << "Failed to open file 'config.json'. Will use default parameters" << std::endl;
        return "";
    }

    // istreambuf_iterator -read the entire file into a string
    std::string jsonString((std::istreambuf_iterator<char>(configFile)),
                           std::istreambuf_iterator<char>());

    configFile.close();
    return jsonString;
}

void ConfigurationManager::setDefaultParameters() {
    Logger::getInstance().getLogger()->info("Setting default configuration parameters");
    displaySimulation = false;
    wallRepresentation = "#";
    dockingStationRepresentation = "D";
    robotRepresentation = "R";
}

void ConfigurationManager::loadParametersFromConfigFile(std::string& jsonString) {
    Logger::getInstance().getLogger()->info("Start processing configuration");
    // Parse the JSON data 
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Check for parse errors 
    if (doc.HasParseError()) {
        Logger::getInstance().getLogger()->error("Parse error: {}.", std::to_string(doc.GetParseError()));
        throw std::runtime_error("Parse error:" + doc.GetParseError());

        //std::cerr << "Parse error: " << doc.GetParseError() << std::endl;
    }

    if (doc.HasMember("displaySimulation") && doc["displaySimulation"].IsBool()) {
        displaySimulation = doc["displaySimulation"].GetBool();
        // std::cout << "Display simulation: " << (displaySimulation ? "true" : "false") << std::endl;
        Logger::getInstance().getLogger()->info("Display simulation" + displaySimulation);
    }

    if (doc.HasMember("representationInSim") && doc["representationInSim"].IsObject()) {
        const rapidjson::Value& simRep = doc["representationInSim"];
        if (simRep.HasMember("wall") && simRep["wall"].IsString()) {
            if (isNotDigit(simRep["wall"].GetString()))
                wallRepresentation = simRep["wall"].GetString();
            // std::cout << "Wall representation in simulation: " << wallRepresentation << std::endl;
            Logger::getInstance().getLogger()->info("Wall representation in simulation:" + wallRepresentation);
        }
        if (simRep.HasMember("dockingStation") && simRep["dockingStation"].IsString()) {
            if (isNotDigit(simRep["dockingStation"].GetString()))
                dockingStationRepresentation = simRep["dockingStation"].GetString();
            // std::cout << "Docking station representation in simulation: " << dockingStationRepresentation << std::endl;
            Logger::getInstance().getLogger()->info("Docking station representation in simulation:" + dockingStationRepresentation);
        }
        if (simRep.HasMember("robot") && simRep["robot"].IsString()) {
            if (isNotDigit(simRep["robot"].GetString()))
                robotRepresentation = simRep["robot"].GetString();
            // std::cout << "robot station representation in simulation: " << robotRepresentation << std::endl;
            Logger::getInstance().getLogger()->info("robot station representation in simulation:" + robotRepresentation);
        }
    }

    if (doc.HasMember("colors") && doc["colors"].IsObject()) {
        const rapidjson::Value& colors = doc["colors"];
        if (colors.HasMember("wall") && colors["wall"].IsString()) {
            colorMapping[Elements::Wall] = colors["wall"].GetString();
            // std::cout << "Wall color: " << colorMapping[Elements::Wall] << std::endl;
            Logger::getInstance().getLogger()->info("Wall color:" + colorMapping[Elements::Wall]);
        }
        if (colors.HasMember("dockingStation") && colors["dockingStation"].IsString()) {
            colorMapping[Elements::DockingStation] = colors["dockingStation"].GetString();
            // std::cout << "Docking station color: " << colorMapping[Elements::DockingStation] << std::endl;
            Logger::getInstance().getLogger()->info("Docking station color:" + colorMapping[Elements::DockingStation]);
        }
        if (colors.HasMember("robot") && colors["robot"].IsString()) {
            colorMapping[Elements::Robot] = colors["robot"].GetString();
            // std::cout << "robot color: " << colorMapping[Elements::Robot] << std::endl;
            Logger::getInstance().getLogger()->info("robot color:" + colorMapping[Elements::Robot]);
        }
        if (colors.HasMember("directionChange") && colors["directionChange"].IsString()) {
            colorMapping[Elements::Direction] = colors["directionChange"].GetString();
            // std::cout << "Direction color: " << colorMapping[Elements::Direction] << std::endl;
            Logger::getInstance().getLogger()->info("Direction color:" + colorMapping[Elements::Direction]);
        }
    }
    Logger::getInstance().getLogger()->info("Done processing configuration");
}

bool ConfigurationManager::getDisplaySim() const {
    return displaySimulation;
}

std::string ConfigurationManager::getWallRep() const {
    return wallRepresentation;
}

std::string ConfigurationManager::getRobotRep() const {
    return robotRepresentation;
}

std::string ConfigurationManager::getStationRep() const {
    return dockingStationRepresentation;
}

/* 
    We dont want to enable a represetation of robot/ docking station/ wall as digit (this belongs to flooe dirt level)
*/
bool ConfigurationManager::isNotDigit(const std::string& str) {
    if(str.empty() || str.size() > 1)
        return true;
    char ch = str[0];
    return !std::isdigit(ch);
}

std::map<Elements, std::string> ConfigurationManager::getColorMapping() const {
    return colorMapping;
}
