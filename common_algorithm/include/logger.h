#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <mutex>

enum class LogLevels {
    CONSOLE,
    FILE,
    ERROR
};

class Logger {
    public:
        static Logger& getInstance();
        void logToConsole(const std::string& message);
        void logToFile(const std::string& message, const std::string& fileName);
        void Logger::logToConsoleError(const std::string& message);
        void log(const std::string& message, LogLevels level);
        
    private:
        Logger(); 
        ~Logger();
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;
        std::mutex mtx; 
};

#endif // LOGGER_H
