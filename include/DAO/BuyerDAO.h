#ifndef DAO_BUYER_DAO_H
#define DAO_BUYER_DAO_H

#include <expected>
#include <memory>
#include <string>

#include "../DTO/BuyerDTO.h"
class BuyerDTO;

class BuyerDAO {
   public:
    // Tương ứng với việc lưu dữ liệu sau khi "create" hoặc "checkout"
    static bool save(const BuyerDTO& buyer);

    // Lấy dữ liệu (Cần thiết để nạp user lên)
    static std::shared_ptr<BuyerDTO> getBuyerById(const std::string& id);
};

#endif
