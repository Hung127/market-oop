#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono>
#include <ctime>
#include <iostream>
#include <functional>

namespace Utils {
    inline std::string getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&nowTime)); 
        return std::string(buffer);
    }

    inline std::string hashPassword(const std::string& password) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(password)); 
    }
}

#endif
