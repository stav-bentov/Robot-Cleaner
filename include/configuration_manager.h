#ifndef CONFIGURATION_MANAGER_H_
#define CONFIGURATION_MANAGER_H_

#include <iostream>
#include <fstream>
#include <string>
#include "rapidjson/include/rapidjson/document.h"
#include "rapidjson/include/rapidjson/filereadstream.h"
#include "common.h"

class ConfigurationManager {
    private:
        bool displaySimulation;
        std::string dockingStationRepresentation;
        std::string robotRepresentation;
        std::string wallRepresentation;
        
        std::map<Elements, std::string> colorMapping = {
            {Elements::Robot, "White"},
            {Elements::DockingStation, "White"},
            {Elements::Direction, "White"},
            {Elements::Wall, "White"},
        };

        std::string readConfig() const;
        void setDefaultParameters();
        void loadParametersFromConfigFile(std::string& jsonString);
        bool isNotDigit(const std::string& str);

    public:
        ConfigurationManager();
        bool getDisplaySim() const;
        std::string getWallRep() const;
        std::string getRobotRep() const;
        std::string getStationRep() const;
        std::map<Elements, std::string> getColorMapping() const;

};
#endif  // CONFIGURATION_MANAGER_H_