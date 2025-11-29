#ifndef DAO_BUYER_DAO_H
#define DAO_BUYER_DAO_H

#include <expected>
#include <memory>
#include <string>

#include "../DTO/Buyer_DTO.h"

class BuyerDao {
   public:
    // Tương ứng với việc lưu dữ liệu sau khi "create" hoặc "checkout"
    static bool save(const BuyerDto& buyer);

    // Lấy dữ liệu (Cần thiết để nạp user lên)
    static std::unique_ptr<BuyerDto> getBuyerById(const std::string& id);
};

#endif