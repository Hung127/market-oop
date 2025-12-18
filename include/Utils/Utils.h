#ifndef UTILS_H
#define UTILS_H
#include <expected>
#include <string>

#include <sodium.h>

namespace Utils {

// Return current date/time as "YYYY-MM-DD hh:mm:ss"
std::string getCurrentDate();

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

class PasswordUtils {
   public:
    // Hash the plain password and return encoded string (contains salt+params+hash).
    static std::expected<std::string, std::string> hash(const std::string& plain);

    // Verify plain against encoded. Returns true if match; false otherwise.
    static bool verify(const std::string& plain, const std::string& encoded);
    inline static const int MIN_PASSWORD_LENGTH = 6;

   private:
    // Ensure libsodium initialized once
    static bool ensureInit();

    // Target limits used by needsRehash
    inline static const unsigned long long OPS_LIMIT = crypto_pwhash_OPSLIMIT_MODERATE;
    inline static const size_t MEM_LIMIT = crypto_pwhash_MEMLIMIT_MODERATE;
};

// TODO: rewrite this function
std::string generateId();

}  // namespace Utils

#endif  // UTILS_H
