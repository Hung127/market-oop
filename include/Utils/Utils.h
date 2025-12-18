#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {

// Return current date/time as "YYYY-MM-DD hh:mm:ss"
std::string getCurrentDate();

// NOTE: This is a placeholder implementation (uses std::hash).
// It is NOT suitable for password storage in production — replace with a proper
// password hashing function (e.g. libsodium / Argon2 / bcrypt / PBKDF2).
std::string hashPassword(const std::string& password);

/**
 * @brief Utility class chứa các thuật toán hỗ trợ tìm kiếm
 *
 * - Levenshtein edit distance
 * - Case-insensitive helpers
 * - Similarity score
 */
class SearchHelper {
   public:
    static int editDistance(const std::string& a, const std::string& b);

    static std::string toLower(const std::string& str);

    static bool containsIgnoreCase(const std::string& text, const std::string& keyword);

    static bool equalsIgnoreCase(const std::string& a, const std::string& b);

    static double similarityScore(const std::string& a, const std::string& b);
};

}  // namespace Utils

#endif  // UTILS_H
