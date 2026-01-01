#include <algorithm>
#include <atomic>
#include <cctype>
#include <chrono>
#include <ctime>
#include <expected>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "../../../include/Utils/Utils.h"

namespace Utils {

std::string getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&nowTime));
    return std::string(buffer);
}

class Password {};

// ----------------- SearchHelper implementations -----------------

int SearchHelper::editDistance(const std::string& a, const std::string& b) {
    int n = static_cast<int>(a.size());
    int m = static_cast<int>(b.size());

    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));

    for (int i = 0; i <= n; ++i)
        dp[i][0] = i;
    for (int j = 0; j <= m; ++j)
        dp[0][j] = j;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({
                                   dp[i - 1][j],     // delete
                                   dp[i][j - 1],     // insert
                                   dp[i - 1][j - 1]  // replace
                               });
            }
        }
    }

    return dp[n][m];
}

std::string SearchHelper::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

bool SearchHelper::containsIgnoreCase(const std::string& text, const std::string& keyword) {
    std::string lowerText = toLower(text);
    std::string lowerKeyword = toLower(keyword);
    return lowerText.find(lowerKeyword) != std::string::npos;
}

bool SearchHelper::equalsIgnoreCase(const std::string& a, const std::string& b) {
    return toLower(a) == toLower(b);
}

double SearchHelper::similarityScore(const std::string& a, const std::string& b) {
    if (a.empty() && b.empty())
        return 1.0;
    if (a.empty() || b.empty())
        return 0.0;

    int distance = editDistance(a, b);
    int maxLen = static_cast<int>(std::max(a.size(), b.size()));
    return 1.0 - (static_cast<double>(distance) / static_cast<double>(maxLen));
}

bool PasswordUtils::ensureInit() {
    static bool initialized = (sodium_init() >= 0);
    return initialized;
}

std::expected<std::string, std::string> PasswordUtils::hash(const std::string& plain) {
    if (!ensureInit()) {
        return std::unexpected("libsodium initialization failed");
    }

    char out[crypto_pwhash_STRBYTES];  // use char[] because the crypto_pwhash_str only accepts
                                       // char[]
    if (crypto_pwhash_str(out, plain.c_str(), plain.size(), OPS_LIMIT, MEM_LIMIT) != 0) {
        return std::unexpected("crypto_pwhash_str failed (out of memory?)");
    }

    return std::string(out);
}

bool PasswordUtils::verify(const std::string& plain, const std::string& encoded) {
    if (encoded.empty()) {
        return false;
    }
    if (!ensureInit()) {
        return false;
    }
    // crypto_pwhash_str_verify returns 0 on success
    return (crypto_pwhash_str_verify(encoded.c_str(), plain.c_str(), plain.size()) == 0);
}

static std::atomic<uint64_t> g_id_counter{0};

std::string generateId() {
    // 1) time in milliseconds since epoch
    auto now_tp = std::chrono::system_clock::now().time_since_epoch();
    uint64_t ms = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now_tp).count());

    // 2) a sequence counter (process-local, thread-safe)
    uint64_t seq = ++g_id_counter;

    // 3) a thread-local RNG for extra entropy between runs/processes
    thread_local std::mt19937_64 rng(std::random_device{}());
    uint64_t rnd = rng();

    // Format as fixed-width hex segments for readability
    std::ostringstream oss;
    oss << std::hex << std::nouppercase << std::setw(12) << std::setfill('0')
        << (ms & 0xFFFFFFFFFFFFULL) << '-'                                   // 12 hex digits
        << std::setw(6) << std::setfill('0') << (seq & 0xFFFFFFULL) << '-'   // 6 hex digits
        << std::setw(12) << std::setfill('0') << (rnd & 0xFFFFFFFFFFFFULL);  // 12 hex digits

    return oss.str();
}



}  // namespace Utils
