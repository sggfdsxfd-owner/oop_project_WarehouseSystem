#include "Logger.h"
#include <ctime>

std::string Logger::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    char buffer[64];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

Logger::Logger(const std::string& filePath)
    : logFilePath(filePath)
{
    logFile.open(logFilePath, std::ios::app);
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

bool Logger::log(const std::string& event) {
    if (!logFile.is_open()) {
        return false;
    }
    logFile << "[" << getCurrentTimestamp() << "] " << event << std::endl;
    return true;
}
