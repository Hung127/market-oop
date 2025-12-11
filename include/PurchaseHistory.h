#ifndef PURCHASEHISTORY_H
#define PURCHASEHISTORY_H

#include <vector>

class OrderDTO;

class PurchaseHistory {
   private:
    std::vector<OrderDTO> _orders;

   public:
    PurchaseHistory() = default;

    // Add an order to history
    void addOrder(const OrderDTO& order);

    // Read-only access to orders
    const std::vector<OrderDTO>& orders() const;

    // Print a human-readable purchase history to stdout
    void printHistory() const;
};

#endif  // PURCHASEHISTORY_H
