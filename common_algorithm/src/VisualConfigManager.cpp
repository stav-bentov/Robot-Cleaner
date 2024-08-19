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
    //Logger::getInstance().getLogger()->info("Setting default configuration parameters");
    displaySimulation = false;
    wallRepresentation = "#";
    dockingStationRepresentation = "D";
    robotRepresentation = "R";
}

void VisualConfigurationManager::loadParametersFromConfigFile(std::string& jsonString){
    //Logger::getInstance().getLogger()->info("Start processing configuration");
    // Parse the JSON data 
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Check for parse errors 
    if (doc.HasParseError()) {
        //Logger::getInstance().getLogger()->error("Parse error: {}.", std::to_string(doc.GetParseError()));
        throw std::runtime_error("Parse error:" + doc.GetParseError());

        //std::cerr << "Parse error: " << doc.GetParseError() << std::endl;
    }

    if (doc.HasMember("displaySimulation") && doc["displaySimulation"].IsBool()) {
        displaySimulation = doc["displaySimulation"].GetBool();
        // std::cout << "Display simulation: " << (displaySimulation ? "true" : "false") << std::endl;
        //Logger::getInstance().getLogger()->info("Display simulation" + displaySimulation);
    }

    if (doc.HasMember("representationInSim") && doc["representationInSim"].IsObject()) {
        const rapidjson::Value& simRep = doc["representationInSim"];
        if (simRep.HasMember("wall") && simRep["wall"].IsString()) {
            if (isNotDigit(simRep["wall"].GetString()))
                wallRepresentation = simRep["wall"].GetString();
            // std::cout << "Wall representation in simulation: " << wallRepresentation << std::endl;
            //Logger::getInstance().getLogger()->info("Wall representation in simulation:" + wallRepresentation);
        }
        if (simRep.HasMember("dockingStation") && simRep["dockingStation"].IsString()) {
            if (isNotDigit(simRep["dockingStation"].GetString()))
                dockingStationRepresentation = simRep["dockingStation"].GetString();
            // std::cout << "Docking station representation in simulation: " << dockingStationRepresentation << std::endl;
            //Logger::getInstance().getLogger()->info("Docking station representation in simulation:" + dockingStationRepresentation);
        }
        if (simRep.HasMember("robot") && simRep["robot"].IsString()) {
            if (isNotDigit(simRep["robot"].GetString()))
                robotRepresentation = simRep["robot"].GetString();
            // std::cout << "robot station representation in simulation: " << robotRepresentation << std::endl;
            //Logger::getInstance().getLogger()->info("robot station representation in simulation:" + robotRepresentation);
        }
    }

    if (doc.HasMember("colors") && doc["colors"].IsObject()) {
        const rapidjson::Value& colors = doc["colors"];
        if (colors.HasMember("wall") && colors["wall"].IsString()) {
            colorMapping[Elements::Wall] = colors["wall"].GetString();
            // std::cout << "Wall color: " << colorMapping[Elements::Wall] << std::endl;
            //Logger::getInstance().getLogger()->info("Wall color:" + colorMapping[Elements::Wall]);
        }
        if (colors.HasMember("dockingStation") && colors["dockingStation"].IsString()) {
            colorMapping[Elements::DockingStation] = colors["dockingStation"].GetString();
            // std::cout << "Docking station color: " << colorMapping[Elements::DockingStation] << std::endl;
            //Logger::getInstance().getLogger()->info("Docking station color:" + colorMapping[Elements::DockingStation]);
        }
        if (colors.HasMember("robot") && colors["robot"].IsString()) {
            colorMapping[Elements::Robot] = colors["robot"].GetString();
            // std::cout << "robot color: " << colorMapping[Elements::Robot] << std::endl;
            //Logger::getInstance().getLogger()->info("robot color:" + colorMapping[Elements::Robot]);
        }
        if (colors.HasMember("directionChange") && colors["directionChange"].IsString()) {
            colorMapping[Elements::Direction] = colors["directionChange"].GetString();
            // std::cout << "Direction color: " << colorMapping[Elements::Direction] << std::endl;
            //Logger::getInstance().getLogger()->info("Direction color:" + colorMapping[Elements::Direction]);
        }
    }
    //Logger::getInstance().getLogger()->info("Done processing configuration");
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
