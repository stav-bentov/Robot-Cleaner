#ifndef VISUAL_CONFIGURATION_MANAGER_H_
#define VISUAL_CONFIGURATION_MANAGER_H_

#include "../include/configuration_manager.h"

class VisualConfigurationManager : public ConfigurationManager {
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

        void setDefaultParameters();
        void loadParametersFromConfigFile(std::string& jsonString);

    public:
        VisualConfigurationManager();
        bool getDisplaySim() const;
        std::string getWallRep() const;
        std::string getRobotRep() const;
        std::string getStationRep() const;
        std::map<Elements, std::string> getColorMapping() const;
};

#endif
