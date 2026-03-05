#ifndef LOGGER_H
#define LOGGER_H

#include <cstdio>
#include <cstdarg>
#include <string>

class Logger
{
public:
    enum class Level
    {
        Info,
        Warning,
        Error
    };

    static void Init(const std::string& filename = "Log.txt");
    static void Shutdown();

    static void Log(Level level, const char* format, ...);

private:
    static FILE* s_LogFile;
};


// Convenience macros
#define LOG_INFO(...)    Logger::Log(Logger::Level::Info, __VA_ARGS__)
#define LOG_WARN(...)    Logger::Log(Logger::Level::Warning, __VA_ARGS__)
#define LOG_ERROR(...)   Logger::Log(Logger::Level::Error, __VA_ARGS__)

#endif
