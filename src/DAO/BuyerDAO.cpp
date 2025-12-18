
#include "../../include/DAO/BuyerDAO.h"

#include <algorithm>
#include <memory>
#include <string>

#include "../../include/DTO/OrderDTO.h"
#include "../../include/DTO/OrderItemDTO.h"

static std::vector<BuyerDTO> mockDatabase;

bool BuyerDAO::save(const BuyerDTO& buyer) {
    // 1. Kiểm tra xem người dùng đã có trong "Database" chưa
    auto it = std::find_if(mockDatabase.begin(), mockDatabase.end(),
                           [&](const BuyerDTO& b) { return b.getId() == buyer.getId(); });

    if (it != mockDatabase.end()) {
        // 2a. Nếu có rồi -> Cập nhật thông tin (Update)
        *it = buyer;  // Ghi đè dữ liệu mới vào
        // std::cout << "[DEBUG] Da cap nhat thong tin Buyer: " << buyer.getName() << "\n";
    } else {
        // 2b. Nếu chưa có -> Thêm mới (Create)
        mockDatabase.push_back(buyer);
        // std::cout << "[DEBUG] Da them Buyer moi: " << buyer.getName() << "\n";
    }

    return true;  // Luôn thành công vì lưu trên RAM không bao giờ lỗi
}

std::shared_ptr<BuyerDTO> BuyerDAO::getBuyerById(const std::string& id) {
    // 1. Duyệt qua kho dữ liệu giả lập
    for (const auto& buyer : mockDatabase) {
        if (buyer.getId() == id) {
            return std::make_shared<BuyerDTO>(buyer);
        }
    }

    // 3. Không tìm thấy
    return nullptr;
}
