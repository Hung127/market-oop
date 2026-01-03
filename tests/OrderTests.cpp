#include <gtest/gtest.h>

#include "TestsCommon.h"

TEST(OrderDAOTest, AddOrderDuplicateAndSellerFilter) {
    // prepare two sellers and products
    auto s1 = makeSeller("od_s1", "OD Seller1");
    auto s2 = makeSeller("od_s2", "OD Seller2");
    auto p1 = makeProduct("od_p1", "ODP1", 7.0, 10, s1);
    auto p2 = makeProduct("od_p2", "ODP2", 9.0, 5, s2);

    // create two order items (use shared_ptr to match OrderDTO ctor signature)
    auto it1 = std::make_shared<OrderItemDTO>(p1->getID(), p1->getName(), p1->getSellerId(),
                                              s1->getName(), p1->getPrice(), 1);
    auto it2 = std::make_shared<OrderItemDTO>(p2->getID(), p2->getName(), p2->getSellerId(),
                                              s2->getName(), p2->getPrice(), 2);

    std::vector<std::shared_ptr<OrderItemDTO>> items1{it1};
    std::vector<std::shared_ptr<OrderItemDTO>> items2{it2};
    auto b1Id = makeBuyer("buyer_x", 50000.0)->getId();
    auto b2Id = makeBuyer("buyer_y", 50000.0)->getId();

    // Construct orders using vector<shared_ptr<OrderItemDTO>> as OrderDTO expects
    OrderDTO order1(Utils::generateId(), b1Id, items1, it1->getSubtotal(), Utils::getCurrentDate());
    OrderDTO order2(Utils::generateId(), b2Id, items2, it2->getSubtotal(), Utils::getCurrentDate());

    ASSERT_TRUE(OrderDAO::addOrder(order1).has_value());
    ASSERT_TRUE(OrderDAO::addOrder(order2).has_value());

    // duplicate id insertion should fail
    EXPECT_FALSE(OrderDAO::addOrder(order1).has_value());

    // getOrdersBySellerId should return correct orders
    auto s1Orders = OrderDAO::getOrdersBySellerId(s1->getId());
    bool found1 = false;
    for (const auto& o : s1Orders) {
        for (const auto& it : o->items()) {
            if (it->getSellerId() == s1->getId()) {
                found1 = true;
            }
        }
    }
    EXPECT_TRUE(found1);

    // getSellerOrderItemByStatus for PENDING (default) should include our items
    auto pendingItemsS1 =
        OrderDAO::getSellerOrderItemByStatus(s1->getId(), OrderItemStatus::PENDING);
    bool contains = false;
    for (const auto& it : pendingItemsS1) {
        if (it.getProductId() == p1->getID()) {
            contains = true;
        }
    }
    EXPECT_TRUE(contains);

    // No need to remove products - they're not referenced by FK anymore
    // The order items store product_id as TEXT, not FK
}

// --------------------------Order status tests-----------------------------------------
/**
 * Main happy-path test:  create full order and run order item through all statuses.
 */
TEST(OrderStatusFlow, FullLifecycle_SuccessSingleItem) {
    auto seller = makeSeller("selleros1", "OrderSeller1");
    auto product = makeProduct("prod_os1", "Widget", 20.0, 10, seller);
    auto buyer = makeBuyer("BuyerOS1", 100.0);

    // Buyer adds to cart and checks out
    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    auto res_ck = BuyerBUS::checkout(*buyer);
    ASSERT_TRUE(res_ck.has_value());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 60.0);  // 100 - (2*20)

    // Find the created order
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];

    ASSERT_TRUE(order != nullptr);
    ASSERT_EQ(order->items().size(), 1U);

    std::string orderId = order->orderId();
    auto item = order->items()[0];

    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Seller confirms
    auto busPack = SellerBUS::create(seller);
    ASSERT_TRUE(busPack.has_value());
    SellerBUS sbus = busPack.value();
    auto confRes = sbus.confirmOrderItem(orderId, product->getID());
    EXPECT_TRUE(confRes.has_value());

    // Reload order to see updated status
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    // Seller ships
    auto shipRes = sbus.shipOrderItem(orderId, product->getID());
    EXPECT_TRUE(shipRes.has_value());

    // Reload order
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    // Seller delivers
    auto delvRes = sbus.deliverOrderItem(orderId, product->getID());
    EXPECT_TRUE(delvRes.has_value());

    // Reload order
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    // No balance/stock changes on confirm/ship/deliver
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 60.0);
    EXPECT_EQ(product->getStock(), 8);

    // Cleanup - delete order first, then product
    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Cancel before seller confirms: refund and restore stock
 */
TEST(OrderStatusFlow, Cancel_BeforeConfirm_RefundsBuyerAndRestoresStock) {
    auto seller = makeSeller("s_os2", "OrderSeller2");
    auto product = makeProduct("prod_os2", "BeforeConfirm", 50.0, 5, seller);
    auto buyer = makeBuyer("BuyerOS2", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 1).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Cancel order item before confirm
    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.cancelOrderItem(orderId, product->getID()).has_value());

    // Reload to check status
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Reload buyer to check refund
    buyer = BuyerDAO::getBuyerById(buyer->getId());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0);

    // Reload product to check stock
    auto productReloaded = ProductDAO::getProductById(product->getID());
    EXPECT_TRUE(productReloaded.has_value());
    EXPECT_EQ(productReloaded.value()->getStock(), 5);

    // Reload order to check total
    order = OrderDAO::getOrderById(orderId);
    EXPECT_DOUBLE_EQ(order->totalPrice(), 0.0);

    // Cleanup
    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Cancel after confirm but before ship
 */
TEST(OrderStatusFlow, Cancel_AfterConfirm_RefundsBuyerAndRestoresStock) {
    auto seller = makeSeller("s_os3", "OrderSeller3");
    auto product = makeProduct("prod_os3", "AfterConfirm", 30.0, 3, seller);
    auto buyer = makeBuyer("BuyerOS3", 200.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    ASSERT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(orderId, product->getID()).has_value());

    // Reload to check confirm
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);
    EXPECT_EQ(product->getStock(), 1);  // 3 - 2

    // Now cancel
    EXPECT_TRUE(sbus.cancelOrderItem(orderId, product->getID()).has_value());

    // Reload to check cancel
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Reload buyer - balance refunded
    buyer = BuyerDAO::getBuyerById(buyer->getId());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 200.0);

    // Reload product - stock restored
    auto productReloaded = ProductDAO::getProductById(product->getID());
    EXPECT_TRUE(productReloaded.has_value());
    EXPECT_EQ(productReloaded.value()->getStock(), 3);

    // Reload order - total recalculated
    order = OrderDAO::getOrderById(orderId);
    EXPECT_DOUBLE_EQ(order->totalPrice(), 0.0);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Cancel after ship:  should fail and not change status
 */
TEST(OrderStatusFlow, Cancel_AfterShip_NotAllowed) {
    auto seller = makeSeller("s_os4", "OrderSeller4");
    auto product = makeProduct("prod_os4", "Shipped", 10.0, 2, seller);
    auto buyer = makeBuyer("BuyerOS4", 50.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(orderId, product->getID()).has_value());
    ASSERT_TRUE(sbus.shipOrderItem(orderId, product->getID()).has_value());

    // Reload to check shipped
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    auto cancelRes = sbus.cancelOrderItem(orderId, product->getID());
    EXPECT_FALSE(cancelRes.has_value());

    // Reload - status unchanged
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    // No refund, no stock update
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 30.0);
    EXPECT_EQ(product->getStock(), 0);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Cancel after delivery: should fail, status unchanged
 */
TEST(OrderStatusFlow, Cancel_AfterDelivery_NotAllowed) {
    auto seller = makeSeller("s_os5", "OrderSeller5");
    auto product = makeProduct("prod_os5", "Delivered", 40.0, 2, seller);
    auto buyer = makeBuyer("BuyerOS5", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(orderId, product->getID()).has_value());
    ASSERT_TRUE(sbus.shipOrderItem(orderId, product->getID()).has_value());
    ASSERT_TRUE(sbus.deliverOrderItem(orderId, product->getID()).has_value());

    // Reload to check delivered
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    auto cancelRes = sbus.cancelOrderItem(orderId, product->getID());
    EXPECT_FALSE(cancelRes.has_value());

    // Reload - status unchanged
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    EXPECT_DOUBLE_EQ(buyer->getBalance(), 20.0);
    EXPECT_EQ(product->getStock(), 0);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Confirm without being seller of the item
 */
TEST(OrderStatusFlow, Confirm_NotYourOrderItem) {
    auto seller1 = makeSeller("s_os6", "SellerC6");
    auto seller2 = makeSeller("s_os62", "SellerC62");  // Different seller!
    auto product = makeProduct("prod_os6", "ConfirmForeign", 77.0, 9, seller1);
    auto buyer = makeBuyer("BuyerOS6", 200.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    EXPECT_EQ(item->getSellerId(), seller1->getId());

    SellerBUS fakeSellerBUS = SellerBUS::create(seller2).value();
    auto confRes = fakeSellerBUS.confirmOrderItem(orderId, product->getID());
    EXPECT_FALSE(confRes.has_value());

    // Reload - order remains pending
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Now real seller confirms
    SellerBUS trueSellerBUS = SellerBUS::create(seller1).value();
    auto confRes2 = trueSellerBUS.confirmOrderItem(orderId, product->getID());
    EXPECT_TRUE(confRes2.has_value());

    // Reload - now confirmed
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Try transitions out of order
 */
TEST(OrderStatusFlow, Transitions_OutOfOrderNotAllowed) {
    auto seller = makeSeller("s_os7", "OrderSeller7");
    auto product = makeProduct("prod_os7", "OutOfOrderFlow", 18.0, 3, seller);
    auto buyer = makeBuyer("BuyerOS7", 200.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 1).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    // Try to ship before confirm
    auto shipRes = sbus.shipOrderItem(orderId, product->getID());
    EXPECT_FALSE(shipRes.has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Try to deliver before confirm/ship
    auto deliverRes = sbus.deliverOrderItem(orderId, product->getID());
    EXPECT_FALSE(deliverRes.has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Confirm, then try to deliver before ship
    ASSERT_TRUE(sbus.confirmOrderItem(orderId, product->getID()).has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    auto deliverRes2 = sbus.deliverOrderItem(orderId, product->getID());
    EXPECT_FALSE(deliverRes2.has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    // Finish the flow
    ASSERT_TRUE(sbus.shipOrderItem(orderId, product->getID()).has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    ASSERT_TRUE(sbus.deliverOrderItem(orderId, product->getID()).has_value());

    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Multi-item order:  Cancel one item but not others, refund partial, recalc total.
 */
TEST(OrderStatusFlow, MultiItemOrder_PartialCancel) {
    auto seller1 = makeSeller("s_mio1", "SellerA");
    auto seller2 = makeSeller("s_mio2", "SellerB");
    auto prodA = makeProduct("p_mio_a", "Alpha", 10.0, 5, seller1);
    auto prodB = makeProduct("p_mio_b", "Beta", 20.0, 3, seller2);
    auto buyer = makeBuyer("BuyerMIO", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, prodA, 2).has_value());
    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, prodB, 1).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    std::string orderId = order->orderId();

    ASSERT_EQ(order->items().size(), 2U);

    auto itemA = order->findItemByProductId(prodA->getID());
    auto itemB = order->findItemByProductId(prodB->getID());
    ASSERT_TRUE(itemA && itemB);

    EXPECT_DOUBLE_EQ(order->totalPrice(), 10.0 * 2 + 20.0 * 1);  // 40. 0

    // Seller1 cancels Alpha, Seller2 confirms and ships Beta
    SellerBUS sbusA = SellerBUS::create(seller1).value();
    SellerBUS sbusB = SellerBUS::create(seller2).value();

    ASSERT_TRUE(sbusA.cancelOrderItem(orderId, prodA->getID()).has_value());

    // Reload to check itemA cancelled
    order = OrderDAO::getOrderById(orderId);
    itemA = order->findItemByProductId(prodA->getID());
    EXPECT_EQ(itemA->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(sbusB.confirmOrderItem(orderId, prodB->getID()).has_value());
    EXPECT_TRUE(sbusB.shipOrderItem(orderId, prodB->getID()).has_value());

    // Reload buyer - should be refunded for prodA:  +20, remaining total = 20
    buyer = BuyerDAO::getBuyerById(buyer->getId());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0 - 20.0);  // Paid 40, got 20 back

    // Reload order - total updated
    order = OrderDAO::getOrderById(orderId);
    EXPECT_DOUBLE_EQ(order->totalPrice(), 20.0);

    // Reload products - stock restored for A, decremented for B
    auto prodAReloaded = ProductDAO::getProductById(prodA->getID());
    EXPECT_TRUE(prodAReloaded.has_value());
    EXPECT_EQ(prodAReloaded.value()->getStock(), 5);
    EXPECT_EQ(prodB->getStock(), 2);

    EXPECT_TRUE(OrderDAO::deleteOrder(orderId));
    EXPECT_TRUE(ProductDAO::remove(prodA->getID()));
    EXPECT_TRUE(ProductDAO::remove(prodB->getID()));
}

/**
 * Edge:  Nonexistent order and product IDs
 */
TEST(OrderStatusFlow, NonexistentIDs_ReturnNotFound) {
    auto seller = makeSeller("nex_sid", "NexSeller");
    SellerBUS sbus = SellerBUS::create(seller).value();

    auto confirmBad = sbus.confirmOrderItem("NON-EXIST-ORDER", "NON-EXIST-PROD");
    EXPECT_FALSE(confirmBad.has_value());
    auto shipBad = sbus.shipOrderItem("NON-EXIST-ORDER", "NON-EXIST-PROD");
    EXPECT_FALSE(shipBad.has_value());
    auto cancelBad = sbus.cancelOrderItem("NON-EXIST-ORDER", "NON-EXIST-PROD");
    EXPECT_FALSE(cancelBad.has_value());
    auto deliverBad = sbus.deliverOrderItem("NON-EXIST-ORDER", "NON-EXIST-PROD");
    EXPECT_FALSE(deliverBad.has_value());
}

/**
 * Edge: Multiple cancels allowed?  (Cancel twice:  second cancel should be no-op or error)
 */
TEST(OrderStatusFlow, DoubleCancel_NotAllowed) {
    auto seller = makeSeller("s_dcanc", "SellerDoubleCancel");
    auto product = makeProduct("prod_dcanc", "DoubleCancel", 14.0, 4, seller);
    auto buyer = makeBuyer("BuyerDC", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    // First cancel succeeds
    ASSERT_TRUE(sbus.cancelOrderItem(orderId, product->getID()).has_value());

    // Reload to check cancelled
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Second cancel fails:  can't cancel an already-cancelled item
    auto result2 = sbus.cancelOrderItem(orderId, product->getID());
    EXPECT_FALSE(result2.has_value());

    // Reload - still cancelled
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Edge:  Confirm/ship/deliver/cancel for non-matching product in the order
 */
TEST(OrderStatusFlow, InvalidProductIdInOrder) {
    auto seller = makeSeller("sid_invprod", "SidInvProd");
    SellerBUS sbus = SellerBUS::create(seller).value();
    auto orderId = "does_not_exist_order";
    auto prodId = "definitely_not_in_order";

    EXPECT_FALSE(sbus.confirmOrderItem(orderId, prodId).has_value());
    EXPECT_FALSE(sbus.cancelOrderItem(orderId, prodId).has_value());
    EXPECT_FALSE(sbus.shipOrderItem(orderId, prodId).has_value());
    EXPECT_FALSE(sbus.deliverOrderItem(orderId, prodId).has_value());
}

/**
 * Edge: Ship/Deliver on cancelled item (should fail, status should stay CANCELLED)
 */
TEST(OrderStatusFlow, ShipDeliver_CancelledItem_NotAllowed) {
    auto seller = makeSeller("sid_cancelled", "SellerCancelledTest");
    auto product = makeProduct("prod_cancelled", "Cancellable", 42.0, 2, seller);
    auto buyer = makeBuyer("BuyerCXL", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 1).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    std::string orderId = order->orderId();

    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    ASSERT_TRUE(sbus.cancelOrderItem(orderId, product->getID()).has_value());

    // Reload to check cancelled
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Now try to ship/deliver the cancelled item
    EXPECT_FALSE(sbus.shipOrderItem(orderId, product->getID()).has_value());

    // Reload - still cancelled
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_FALSE(sbus.deliverOrderItem(orderId, product->getID()).has_value());

    // Reload - still cancelled
    order = OrderDAO::getOrderById(orderId);
    item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}
