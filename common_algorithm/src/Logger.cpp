#include "../include/logger.h"

Logger::~Logger() {}
Logger::Logger() {}

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
    logFile.close();
}

void Logger::logToConsoleError(const std::string& message)  {
    std::lock_guard<std::mutex> lock(mtx);
    std::cerr << message << std::endl;
}

void Logger::log(const std::string& message, LogLevels level) {
    switch (level)
    {
        case LogLevels::CONSOLE:
            logToConsole(message);
            break;
        case LogLevels::FILE:
            logToFile(message, "log.txt");
            break;
        case LogLevels::ERROR:
            logToConsoleError(message);
            break;
        
        default:
            break;
    }
}
