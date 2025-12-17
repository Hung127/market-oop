#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <ctime>
#include <functional>
#include <iostream>
#include <string>

// TODO: separate headers and implementation
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

/**
 * @brief Utility class chứa các thuật toán hỗ trợ tìm kiếm
 *
 * Class này chứa các static methods để:
 * - Tính Edit Distance (Levenshtein Distance) cho fuzzy search
 * - Xử lý chuỗi (lowercase, case-insensitive comparison)
 */
class SearchHelper {
   public:
    /**
     * @brief Tính Edit Distance (Levenshtein Distance) giữa 2 chuỗi
     *
     * Edit Distance là số thao tác tối thiểu (insert, delete, replace)
     * để chuyển chuỗi a thành chuỗi b.
     *
     * Sử dụng Dynamic Programming với độ phức tạp O(m*n)
     *
     * Ví dụ:
     * - editDistance("kitten", "sitting") = 3
     *   kitten → sitten (replace k→s)
     *   sitten → sittin (replace e→i)
     *   sittin → sitting (insert g)
     *
     * - editDistance("laptop", "lapto") = 1 (delete p)
     * - editDistance("mouse", "mose") = 1 (delete u)
     *
     * @param a Chuỗi nguồn
     * @param b Chuỗi đích
     * @return Số thao tác tối thiểu cần thực hiện
     */
    static int editDistance(const std::string& a, const std::string& b) {
        int n = a.size();
        int m = b.size();

        // Tạo bảng DP kích thước (n+1) x (m+1)
        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1));

        // Base cases:
        // - Chuyển chuỗi rỗng thành chuỗi b cần m thao tác insert
        // - Chuyển chuỗi a thành chuỗi rỗng cần n thao tác delete
        for (int i = 0; i <= n; i++) {
            dp[i][0] = i;  // Delete all characters from a
        }
        for (int j = 0; j <= m; j++) {
            dp[0][j] = j;  // Insert all characters of b
        }

        // Fill DP table
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                if (a[i - 1] == b[j - 1]) {
                    // Characters match, no operation needed
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    // Characters don't match, take minimum of:
                    // 1. dp[i-1][j] + 1   : Delete from a
                    // 2. dp[i][j-1] + 1   : Insert into a
                    // 3. dp[i-1][j-1] + 1 : Replace in a
                    dp[i][j] = 1 + std::min({
                                       dp[i - 1][j],     // Delete
                                       dp[i][j - 1],     // Insert
                                       dp[i - 1][j - 1]  // Replace
                                   });
                }
            }
        }

        return dp[n][m];
    }

    /**
     * @brief Chuyển chuỗi sang lowercase
     *
     * Hữu ích cho case-insensitive search.
     *
     * @param str Chuỗi đầu vào
     * @return Chuỗi lowercase
     */
    static std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    /**
     * @brief Kiểm tra chuỗi text có chứa keyword không (case-insensitive)
     *
     * Ví dụ:
     * - containsIgnoreCase("Dell Laptop", "laptop") → true
     * - containsIgnoreCase("Dell Laptop", "DELL") → true
     * - containsIgnoreCase("Mouse", "keyboard") → false
     *
     * @param text Chuỗi chính cần tìm trong đó
     * @param keyword Từ khóa cần tìm
     * @return true nếu text chứa keyword (không phân biệt hoa thường)
     */
    static bool containsIgnoreCase(const std::string& text, const std::string& keyword) {
        std::string lowerText = toLower(text);
        std::string lowerKeyword = toLower(keyword);
        return lowerText.find(lowerKeyword) != std::string::npos;
    }

    /**
     * @brief So sánh 2 chuỗi (case-insensitive)
     *
     * @param a Chuỗi thứ nhất
     * @param b Chuỗi thứ hai
     * @return true nếu 2 chuỗi giống nhau (không phân biệt hoa thường)
     */
    static bool equalsIgnoreCase(const std::string& a, const std::string& b) {
        return toLower(a) == toLower(b);
    }

    /**
     * @brief Tính similarity score giữa 2 chuỗi (0.0 - 1.0)
     *
     * Score càng cao = càng giống nhau
     * - 1.0: Hoàn toàn giống nhau
     * - 0.0: Hoàn toàn khác nhau
     *
     * Formula: 1 - (editDistance / max(len(a), len(b)))
     *
     * @param a Chuỗi thứ nhất
     * @param b Chuỗi thứ hai
     * @return Similarity score từ 0.0 đến 1.0
     */
    static double similarityScore(const std::string& a, const std::string& b) {
        if (a.empty() && b.empty())
            return 1.0;
        if (a.empty() || b.empty())
            return 0.0;

        int distance = editDistance(a, b);
        int maxLen = std::max(a.size(), b.size());

        return 1.0 - (static_cast<double>(distance) / maxLen);
    }
};
}  // namespace Utils

#endif
