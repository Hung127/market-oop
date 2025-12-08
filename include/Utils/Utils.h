#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <ctime>
#include <string>

namespace Utils {
inline std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    char buffer[20];
    // Vì bạn đang dùng Windows (VS Code), localtime thường bị warning unsafe
    // NOLINT giúp tắt warning đó, hoặc dùng localtime_s nếu muốn chuẩn chỉ
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", std::localtime(&nowTime));  // NOLINT
    return std::string(buffer);
}
}  // namespace Utils

#endif
