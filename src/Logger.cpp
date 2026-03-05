#include "Logger.h"
#include <ctime>

FILE* Logger::s_LogFile = nullptr;

void Logger::Init(const std::string& filename)
{
    s_LogFile = fopen(filename.c_str(), "w");
    if (s_LogFile == NULL) {
        fprintf(stderr, "Failed to create log file: %s\n", filename.c_str());
        exit(EXIT_FAILURE);
    }
}

void Logger::Shutdown()
{
    if (s_LogFile)
    {
        fclose(s_LogFile);
        s_LogFile = nullptr;
    }
}

void Logger::Log(Level level, const char* format, ...)
{
    if (!s_LogFile)
        return;

    const char* levelStr = "";

    switch (level)
    {
        case Level::Info:    levelStr = "[INFO] "; break;
        case Level::Warning: levelStr = "[WARN] "; break;
        case Level::Error:   levelStr = "[ERROR] "; break;
    }

    fprintf(s_LogFile, "%s", levelStr);

    va_list args;
    va_start(args, format);
    vfprintf(s_LogFile, format, args);
    va_end(args);

    fprintf(s_LogFile, "\n");
    fflush(s_LogFile);
}
