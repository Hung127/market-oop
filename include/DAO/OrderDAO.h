#ifndef ORDER_DAO_H
#define ORDER_DAO_H

#include <expected>
#include <memory>

#include "../DTO/OrderDTO.h"

class OrderDAO {
   private:
    inline static std::vector<OrderDTO> _orders;

   public:
    static std::expected<void, std::string> addOrder(const OrderDTO& order);

    static std::shared_ptr<OrderDTO> getOrderById(const std::string& id);

    static std::vector<std::shared_ptr<OrderDTO>> getOrdersByBuyerId(const std::string& id);

    static std::vector<std::shared_ptr<OrderDTO>> getOrdersBySellerId(const std::string& id);
};

#endif
