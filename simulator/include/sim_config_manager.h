#ifndef SIM_CONFIGURATION_MANAGER_H_
#define SIM_CONFIGURATION_MANAGER_H_

#include "../../common_algorithm/include/configuration_manager.h"

class SimConfigurationManager : public ConfigurationManager {
    private:
        int timePerStep;
        void setDefaultParameters();
        void loadParametersFromConfigFile(std::string& jsonString);

    public:
        SimConfigurationManager();
        int getTimePerStep() const;
};

#endif
