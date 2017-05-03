#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <mutex>

#define LOG Logger()

struct Logger {
    Logger() {}

    ~Logger();

    Logger &operator<<(const std::string &s) {
        m_LogLines.push_back(s);
        return *this;
    }

    static std::mutex m_Mutex;

private:
    std::vector<std::string> m_LogLines;
};

#endif // LOGGER_H
