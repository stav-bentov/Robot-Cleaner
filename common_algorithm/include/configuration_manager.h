#ifndef CONFIGURATION_MANAGER_H_
#define CONFIGURATION_MANAGER_H_

#include <iostream>
#include <fstream>
#include <string>
#include "../../libs/rapidjson/include/rapidjson/document.h"
#include "../../libs/rapidjson/include/rapidjson/filereadstream.h"
#include "common_enums.h"

class ConfigurationManager {
    protected:
        std::string readConfig(const std::string& configFileName) const;
        virtual void loadParametersFromConfigFile(std::string& jsonString) = 0;
        bool isNotDigit(const std::string& str);
    public:
        ConfigurationManager();
        virtual ~ConfigurationManager() = default; // Virtual destructor
};
#endif  // CONFIGURATION_MANAGER_H_