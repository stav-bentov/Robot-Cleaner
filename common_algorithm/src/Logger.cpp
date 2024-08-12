#include "../include/logger.h"

Logger::Logger() : logLevel(3) {}

Logger::~Logger() {}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::logToConsole(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << message << std::endl;
}

void Logger::logToFile(const std::string& message, const std::string& fileName) {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream logFile(fileName, std::ios_base::app);
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

void Logger::setLogLevel(int level) {
    logLevel = level;
}

void Logger::log(const std::string& message, int level) {
    switch (level)
    {
        case 2:
            logToConsole(message);
            break;
        case 3:
            logToFile(message, "log.txt");
            break;
        
        default:
            break;
    }
}
