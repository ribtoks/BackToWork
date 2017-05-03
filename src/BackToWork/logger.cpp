#include "logger.h"

#include <iostream>
#include <algorithm>
#include <ctime>

std::mutex Logger::m_Mutex;

Logger::~Logger() {
    std::lock_guard<std::mutex> guard(m_Mutex);
    
    std::time_t now = std::time(NULL);
    std::string timeStr(std::ctime(&now));
    timeStr.erase(std::remove(timeStr.begin(), timeStr.end(), '\n'), timeStr.end());
    std::cout << timeStr << " - ";
        
    for (auto &s: m_LogLines) {
        std::cout << s << ' ';
    }

    std::cout << std::endl;
}
