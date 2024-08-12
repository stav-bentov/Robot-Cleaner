#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <mutex>

class Logger {
public:
    // Get the single instance of Logger
    static Logger& getInstance();

    // Log messages to the console
    void logToConsole(const std::string& message);

    // Log messages to a file
    void logToFile(const std::string& message, const std::string& fileName);

    // Set logging level
    void setLogLevel(int level);

    // Log messages depending on log level
    void log(const std::string& message, int level);

private:
    Logger(); 
    ~Logger();

    // Disable copy and assignment
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    int logLevel;
    std::mutex mtx; 
};

#endif // LOGGER_H
