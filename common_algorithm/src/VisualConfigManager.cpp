#include "../include/visualisation_configuration_manager.h"


VisualConfigurationManager::VisualConfigurationManager() 
    : displaySimulation(false),
      dockingStationRepresentation("D"),  
      robotRepresentation("R"),            
      wallRepresentation("#") {
    setDefaultParameters();
    std::string jsonString = readConfig("config/visualisation_config.json");
    // Default parameters...
    if (jsonString == "")
        return;
    loadParametersFromConfigFile(jsonString);
}

void VisualConfigurationManager::setDefaultParameters() {
    displaySimulation = false;
    wallRepresentation = "#";
    dockingStationRepresentation = "D";
    robotRepresentation = "R";
}

void VisualConfigurationManager::loadParametersFromConfigFile(std::string& jsonString){
    // Parse the JSON data 
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Check for parse errors 
    if (doc.HasParseError()) {
        throw std::runtime_error("Parse error:" + doc.GetParseError());
    }

    if (doc.HasMember("displaySimulation") && doc["displaySimulation"].IsBool()) {
        displaySimulation = doc["displaySimulation"].GetBool();
    }

    if (doc.HasMember("representationInSim") && doc["representationInSim"].IsObject()) {
        const rapidjson::Value& simRep = doc["representationInSim"];
        if (simRep.HasMember("wall") && simRep["wall"].IsString()) {
            if (isNotDigit(simRep["wall"].GetString()))
                wallRepresentation = simRep["wall"].GetString();
        }
        if (simRep.HasMember("dockingStation") && simRep["dockingStation"].IsString()) {
            if (isNotDigit(simRep["dockingStation"].GetString()))
                dockingStationRepresentation = simRep["dockingStation"].GetString();
        }
        if (simRep.HasMember("robot") && simRep["robot"].IsString()) {
            if (isNotDigit(simRep["robot"].GetString()))
                robotRepresentation = simRep["robot"].GetString();
        }
    }

    if (doc.HasMember("colors") && doc["colors"].IsObject()) {
        const rapidjson::Value& colors = doc["colors"];
        if (colors.HasMember("wall") && colors["wall"].IsString()) {
            colorMapping[Elements::Wall] = colors["wall"].GetString();
        }
        if (colors.HasMember("dockingStation") && colors["dockingStation"].IsString()) {
            colorMapping[Elements::DockingStation] = colors["dockingStation"].GetString();
        }
        if (colors.HasMember("robot") && colors["robot"].IsString()) {
            colorMapping[Elements::Robot] = colors["robot"].GetString();
        }
        if (colors.HasMember("directionChange") && colors["directionChange"].IsString()) {
            colorMapping[Elements::Direction] = colors["directionChange"].GetString();
        }
    }
}

bool VisualConfigurationManager::getDisplaySim() const {
    return displaySimulation;
}

std::string VisualConfigurationManager::getWallRep() const {
    return wallRepresentation;
}

std::string VisualConfigurationManager::getRobotRep() const {
    return robotRepresentation;
}

std::string VisualConfigurationManager::getStationRep() const {
    return dockingStationRepresentation;
}

std::map<Elements, std::string> VisualConfigurationManager::getColorMapping() const {
    return colorMapping;
}
