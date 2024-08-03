#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <memory>
#include <iostream>

class Logger {
    // Singleton class for logger
public:
    static Logger& getInstance();
    spdlog::logger* getLogger();

private:
    Logger();  // Private constructor (because singleton)
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // The object that will be returned
    std::shared_ptr<spdlog::logger> logger;
};

#endif // LOGGER_H