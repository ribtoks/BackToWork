#include "logger.h"

#include <iostream>
#include <iomanip>
#include <ctime>

std::mutex Logger::m_Mutex;

Logger::~Logger() {
    std::lock_guard<std::mutex> guard(m_Mutex);

    auto t = std::time(nullptr);
	tm tm;
	localtime_s(&tm, &t);
    std::cout << std::put_time(&tm, "%H-%M-%S") << " - ";

    for (auto &s: m_LogLines) {
        std::cout << s << ' ';
    }

    std::cout << std::endl;
}
