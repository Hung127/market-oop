#ifndef PURCHASEHISTORY_DTO_H
#define PURCHASEHISTORY_DTO_H

#include <vector>

#include "OrderDTO.h"

class PurchaseHistoryDTO {
   private:
    std::vector<OrderDTO> _orders;

   public:
    PurchaseHistoryDTO() = default;

    // Add an order to history
    void addOrder(const OrderDTO& order);

    // Read-only access to orders
    const std::vector<OrderDTO>& orders() const;

    // Print a human-readable purchase history to stdout
    // TODO: move it to ui
    void printHistory() const;
};

#endif  // PURCHASEHISTORY_H
