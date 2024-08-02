#include "../include/logger.h"

/*
    Initialize the logger with a file sink (every print to logger will come to this file)
*/
Logger::Logger() {
    try {
        logger = spdlog::basic_logger_mt("file_logger", "Log/logfile.txt");
        spdlog::flush_on(spdlog::level::info);  // Ensure logs are flushed on info level
    } catch (const spdlog::spdlog_ex& ex) {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

/*
    Flush and close the logger before destruction
*/
Logger::~Logger() {
    spdlog::drop_all();
}

/*
    Get the singleton instance of the Logger
*/
Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

/*
    Get the logger instance
*/
spdlog::logger* Logger::getLogger() {
    return logger.get();
}