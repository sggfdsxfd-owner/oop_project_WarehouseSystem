#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

class Logger {
private:
    std::string logFilePath;
    std::ofstream logFile;

    static std::string getCurrentTimestamp();

public:
    explicit Logger(const std::string& filePath = "data/system.log");
    ~Logger();

    bool log(const std::string& event);
};

#endif // LOGGER_H
