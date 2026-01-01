#include <gtest/gtest.h>

#include "TestsCommon.h"

// ORDER DAO: add order, duplicate id, get by seller id filtering
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

    // Construct orders using vector<shared_ptr<OrderItemDTO>> as OrderDTO expects
    OrderDTO order1(Utils::generateId(), "buyer_x", items1, it1->getSubtotal(),
                    Utils::getCurrentDate());
    OrderDTO order2(Utils::generateId(), "buyer_y", items2, it2->getSubtotal(),
                    Utils::getCurrentDate());

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

    // cleanup products
    EXPECT_TRUE(ProductDAO::remove(p1->getID()));
    EXPECT_TRUE(ProductDAO::remove(p2->getID()));
}

// --------------------------Order status tests-----------------------------------------
/**
 * Main happy-path test: create full order and run order item through all statuses.
 */
TEST(OrderStatusFlow, FullLifecycle_SuccessSingleItem) {
    auto seller = makeSeller("selleros1", "OrderSeller1");
    auto product = makeProduct("prod_os1", "Widget", 20.0, 10, seller);
    auto buyer = makeBuyer("BuyerOS1", 100.0);

    // Buyer adds to cart and checks out
    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    auto res_ck = BuyerBUS::payment(*buyer, {product->getID()}, {}, false);
    ASSERT_TRUE(res_ck.has_value());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 60.0);  // 100 - (2*20)

    // Find the created order
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];

    ASSERT_TRUE(order != nullptr);
    ASSERT_EQ(order->items().size(), 1U);
    auto item = order->items()[0];

    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Seller confirms
    auto busPack = SellerBUS::create(seller);
    ASSERT_TRUE(busPack.has_value());
    SellerBUS sbus = busPack.value();
    auto confRes = sbus.confirmOrderItem(order->orderId(), product->getID());
    EXPECT_TRUE(confRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    // Seller ships
    auto shipRes = sbus.shipOrderItem(order->orderId(), product->getID());
    EXPECT_TRUE(shipRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    // Seller delivers
    auto delvRes = sbus.deliverOrderItem(order->orderId(), product->getID());
    EXPECT_TRUE(delvRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    // No balance/stock changes on confirm/ship/deliver
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 60.0);
    EXPECT_EQ(product->getStock(), 8);

    // Cleanup
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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    auto item = order->items()[0];
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Cancel order item before confirm
    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.cancelOrderItem(order->orderId(), product->getID()).has_value());

    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Buyer refunded full item price
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0);

    // Stock restored
    EXPECT_EQ(product->getStock(), 5);

    // Order total recalculated to 0
    EXPECT_DOUBLE_EQ(order->totalPrice(), 0.0);

    // Cleanup
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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    auto item = order->items()[0];
    ASSERT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);
    EXPECT_EQ(product->getStock(), 1);  // 3 - 2

    // Now cancel
    EXPECT_TRUE(sbus.cancelOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Balance refunded
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 200.0);
    // Stock restored
    EXPECT_EQ(product->getStock(), 3);

    // Order total recalculated
    EXPECT_DOUBLE_EQ(order->totalPrice(), 0.0);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Cancel after ship: should fail and not change status
 */
TEST(OrderStatusFlow, Cancel_AfterShip_NotAllowed) {
    auto seller = makeSeller("s_os4", "OrderSeller4");
    auto product = makeProduct("prod_os4", "Shipped", 10.0, 2, seller);
    auto buyer = makeBuyer("BuyerOS4", 50.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    auto item = order->items()[0];

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(order->orderId(), product->getID()).has_value());
    ASSERT_TRUE(sbus.shipOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    auto cancelRes = sbus.cancelOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(cancelRes.has_value());
    // Status unchanged
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);
    // No refund, no stock update
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 30.0);
    EXPECT_EQ(product->getStock(), 0);

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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    auto item = order->items()[0];

    SellerBUS sbus = SellerBUS::create(seller).value();
    ASSERT_TRUE(sbus.confirmOrderItem(order->orderId(), product->getID()).has_value());
    ASSERT_TRUE(sbus.shipOrderItem(order->orderId(), product->getID()).has_value());
    ASSERT_TRUE(sbus.deliverOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    auto cancelRes = sbus.cancelOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(cancelRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 20.0);
    EXPECT_EQ(product->getStock(), 0);

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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    auto item = order->items()[0];
    EXPECT_EQ(item->getSellerId(), seller1->getId());

    SellerBUS fakeSellerBUS = SellerBUS::create(seller2).value();
    auto confRes = fakeSellerBUS.confirmOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(confRes.has_value());
    // Order remains pending
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Now real seller confirms
    SellerBUS trueSellerBUS = SellerBUS::create(seller1).value();
    auto confRes2 = trueSellerBUS.confirmOrderItem(order->orderId(), product->getID());
    EXPECT_TRUE(confRes2.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    // Try to ship before confirm
    auto shipRes = sbus.shipOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(shipRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Try to deliver before confirm/ship
    auto deliverRes = sbus.deliverOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(deliverRes.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::PENDING);

    // Confirm, then try to deliver before ship
    ASSERT_TRUE(sbus.confirmOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    auto deliverRes2 = sbus.deliverOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(deliverRes2.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CONFIRMED);

    // Finish the flow
    ASSERT_TRUE(sbus.shipOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::SHIPPED);

    ASSERT_TRUE(sbus.deliverOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::DELIVERED);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Multi-item order: Cancel one item but not others, refund partial, recalc total.
 */
TEST(OrderStatusFlow, MultiItemOrder_PartialCancel) {
    auto seller1 = makeSeller("s_mio1", "SellerA");
    auto seller2 = makeSeller("s_mio2", "SellerB");
    auto prodA = makeProduct("p_mio_a", "Alpha", 10.0, 5, seller1);
    auto prodB = makeProduct("p_mio_b", "Beta", 20.0, 3, seller2);
    auto buyer = makeBuyer("BuyerMIO", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, prodA, 2).has_value());
    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, prodB, 1).has_value());
    std::vector<std::string> ids = {prodA->getID(), prodB->getID()};
    ASSERT_TRUE(BuyerBUS::payment(*buyer, ids, {}, false).has_value());

    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    ASSERT_EQ(orders.size(), 1U);
    auto order = orders[0];
    ASSERT_EQ(order->items().size(), 2U);

    auto itemA = order->findItemByProductId(prodA->getID());
    auto itemB = order->findItemByProductId(prodB->getID());
    ASSERT_TRUE(itemA && itemB);

    EXPECT_DOUBLE_EQ(order->totalPrice(), 10.0 * 2 + 20.0 * 1);  // 40.0

    // Seller1 cancels Alpha, Seller2 confirms and ships Beta
    SellerBUS sbusA = SellerBUS::create(seller1).value();
    SellerBUS sbusB = SellerBUS::create(seller2).value();

    ASSERT_TRUE(sbusA.cancelOrderItem(order->orderId(), prodA->getID()).has_value());
    EXPECT_EQ(itemA->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(sbusB.confirmOrderItem(order->orderId(), prodB->getID()).has_value());
    EXPECT_TRUE(sbusB.shipOrderItem(order->orderId(), prodB->getID()).has_value());

    // Buyer should be refunded for prodA: +20, remaining total = 20
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0 - 20.0);  // Paid 40, got 20 back

    EXPECT_DOUBLE_EQ(order->totalPrice(), 20.0);

    // Stock restored for A, decremented for B
    EXPECT_EQ(prodA->getStock(), 5);
    EXPECT_EQ(prodB->getStock(), 2);

    EXPECT_TRUE(ProductDAO::remove(prodA->getID()));
    EXPECT_TRUE(ProductDAO::remove(prodB->getID()));
}

/**
 * Edge: Nonexistent order and product IDs
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
 * Edge: Multiple cancels allowed? (Cancel twice: second cancel should be no-op or error)
 */
TEST(OrderStatusFlow, DoubleCancel_NotAllowed) {
    auto seller = makeSeller("s_dcanc", "SellerDoubleCancel");
    auto product = makeProduct("prod_dcanc", "DoubleCancel", 14.0, 4, seller);
    auto buyer = makeBuyer("BuyerDC", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    // First cancel succeeds
    ASSERT_TRUE(sbus.cancelOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Second cancel fails: can't cancel an already-cancelled item
    auto result2 = sbus.cancelOrderItem(order->orderId(), product->getID());
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));
}

/**
 * Edge: Confirm/ship/deliver/cancel for non-matching product in the order
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
    ASSERT_TRUE(BuyerBUS::payment(*buyer, {product->getID()}, {}, false).has_value());
    auto orders = OrderDAO::getOrdersByBuyerId(buyer->getId());
    auto order = orders[0];
    auto item = order->items()[0];
    SellerBUS sbus = SellerBUS::create(seller).value();

    ASSERT_TRUE(sbus.cancelOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    // Now try to ship/deliver the cancelled item
    EXPECT_FALSE(sbus.shipOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_FALSE(sbus.deliverOrderItem(order->orderId(), product->getID()).has_value());
    EXPECT_EQ(item->getStatus(), OrderItemStatus::CANCELLED);

    EXPECT_TRUE(ProductDAO::remove(product->getID()));


}


TEST(BuyerBUSTest, CheckoutSuccessAndHistory) {
    // setup seller and product
    auto seller = makeSeller("s_buyer1", "BuyerSeller1");
    auto prod_ptr = std::make_shared<ProductDTO>("p_buy1", "BuyItem", 10.0, 5, seller);
    ASSERT_TRUE(ProductDAO::insert(prod_ptr));

    // create buyer with enough balance
    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "TestBuyer", "tb@example.com",
                                           "pwdjsfaklpjf", 100.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    ASSERT_NE(buyer, nullptr);

    // add to cart and checkout
    auto a1 = BuyerBUS::addToCart(*buyer, prod_ptr, 2);
    ASSERT_TRUE(a1.has_value());

 // THÊM CÁC BIẾN NÀY ĐỂ TRUYỀN VÀO HÀM CHECKOUT
    std::vector<std::string> selectedIds = {prod_ptr->getID()}; 
    std::vector<VoucherDTO*> emptyVouchers; // Danh sách voucher rỗng
    bool useCoins = false; // Không dùng xu

    // Cập nhật lời gọi hàm với 4 tham số
    auto ck = BuyerBUS::finalOrder(true,*buyer, selectedIds, emptyVouchers, useCoins);
    EXPECT_TRUE(ck.has_value());

    // balance decreased
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0 - 20.0);

    // product stock decreased in DAO instance too (get by id)
    auto fetched = ProductDAO::getProductById("p_buy1");
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched.value()->getStock(), 3);

    // purchase history contains 1 order
    const auto& history = buyer->getPurchasesHistory().orders();
    ASSERT_EQ(history.size(), 1U);
    EXPECT_EQ(history[0].totalPrice(), 20.0);

    // cleanup
    EXPECT_TRUE(ProductDAO::remove("p_buy1"));
}

TEST(BuyerBUSTest, CheckoutInsufficientBalance) {
    auto seller = makeSeller("s_buyer2", "BuyerSeller2");
    auto prod_ptr = std::make_shared<ProductDTO>("p_buy21", "Expensive", 50.0, 3, seller);
    ASSERT_TRUE(ProductDAO::insert(*prod_ptr));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "PoorBuyer", "ipoor@example.com",
                                           "pwsdajkflasdjklfjl", 10.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    ASSERT_NE(buyer, nullptr);

    auto addRes = BuyerBUS::addToCart(*buyer, prod_ptr, 1);
    ASSERT_TRUE(addRes.has_value());

// THÊM CÁC BIẾN NÀY ĐỂ TRUYỀN VÀO HÀM CHECKOUT
    std::vector<std::string> selectedIds = {prod_ptr->getID()};
    std::vector<VoucherDTO*> emptyVouchers;
    bool useCoins = false;

    // Cập nhật lời gọi hàm với 4 tham số
    auto ck = BuyerBUS::finalOrder(true,*buyer, selectedIds, emptyVouchers, useCoins);
    EXPECT_FALSE(ck.has_value());  // should fail

    // ensure nothing changed: balance unchanged, stock unchanged
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 10.0);
    auto fetched = ProductDAO::getProductById("p_buy21");
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched.value()->getStock(), 3);

    // cleanup
    EXPECT_TRUE(ProductDAO::remove("p_buy21"));
}

// Test: Kiểm tra trừ tồn kho (Stock) sau khi thanh toán thành công
TEST(BuyerBUSTest, StockReducedAfterFinalOrder) {
    auto seller = makeSeller("s_stock_check", "StockCheckSeller");
    // Sản phẩm ban đầu có 10 món
    auto prod = std::make_shared<ProductDTO>("p_stk", "StockItem", 100.0, 10, seller);
    ProductDAO::insert(prod);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "StkUser", "stk@test.com", "pw123456", 500.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    
    // Mua 3 món
    BuyerBUS::addToCart(*buyer, prod, 3);

    // Thực hiện chốt đơn
    auto res = BuyerBUS::finalOrder(true, *buyer, {"p_stk"}, {}, false);
    ASSERT_TRUE(res.has_value());

    // Kiểm tra kho: 10 - 3 = 7
    auto fetched = ProductDAO::getProductById("p_stk");
    EXPECT_EQ(fetched.value()->getStock(), 7); 

    ProductDAO::remove("p_stk");
}

// Test: Thanh toán thất bại khi hàng trong kho không đủ
TEST(BuyerBUSTest, FinalOrderFailsWhenOutOffStock) {
    auto seller = makeSeller("s_out", "OutOfStockSeller");
    // Sản phẩm chỉ còn 2 món
    auto prod = std::make_shared<ProductDTO>("p_out", "LimitedItem", 100.0, 2, seller);
    ProductDAO::insert(prod);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "UserOut", "out@test.com", "pw123456", 500.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    
    // Cố tình thêm 5 món (vượt quá tồn kho 2)
    // Lưu ý: CartBUS::add có thể cho phép thêm, nhưng payment/finalOrder sẽ chặn lại
    BuyerBUS::addToCart(*buyer, prod, 5);

    // Chốt đơn phải trả về lỗi
    auto res = BuyerBUS::finalOrder(true, *buyer, {"p_out"}, {}, false);
    EXPECT_FALSE(res.has_value()); 

    // Kiểm tra kho vẫn giữ nguyên là 2
    EXPECT_EQ(prod->getStock(), 2);

    ProductDAO::remove("p_out");
}

// ----------------------------- Voucher and Coin tests and Stock -----------------------------


// Test 1: Thanh toán có áp dụng Seller Voucher thành công
TEST(BuyerBUSTest, CheckoutWithVoucherSuccess) {
    auto seller = makeSeller("s_vouch", "VoucherSeller");
    // Sản phẩm giá 100.0
    auto prod = std::make_shared<ProductDTO>("p_vouch", "VoucherItem", 100.0, 10, seller);
    ASSERT_TRUE(ProductDAO::insert(prod));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "VoucherUser", "vu@test.com", "pw123456", 200.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());

    BuyerBUS::addToCart(*buyer, prod, 1);

    // Tạo Voucher: Giảm 10%, yêu cầu đơn tối thiểu 50.0
    SellerVoucherDTO voucher("v_test", "SALE10", "s_vouch", 10.0, 50.0, "2030-12-31 23:59:59");
    std::vector<VoucherDTO*> vouchers = { &voucher };
    std::vector<std::string> selectedIds = {"p_vouch"};

    auto ck = BuyerBUS::finalOrder(true,*buyer, selectedIds, vouchers, false);
    ASSERT_TRUE(ck.has_value()) << (ck.has_value() ? "" : ck.error());

    // Kiểm tra số dư: 200 - (100 - 10%*100) = 200 - 90 = 110
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 110.0);

    EXPECT_TRUE(ProductDAO::remove("p_vouch"));
}

// Test 2: Voucher không được áp dụng do không đủ giá trị đơn hàng tối thiểu
TEST(BuyerBUSTest, CheckoutWithVoucherFailsMinOrder) {
    auto seller = makeSeller("s_vouch_fail", "VoucherFailSeller");
    // Sản phẩm giá 30.0
    auto prod = std::make_shared<ProductDTO>("p_vouch_fail", "CheapItem", 30.0, 10, seller);
    ASSERT_TRUE(ProductDAO::insert(prod));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "VoucherUser2", "vu2@test.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());

    BuyerBUS::addToCart(*buyer, prod, 1);

    // Voucher yêu cầu min 50.0 -> Đơn hàng 30.0 sẽ không được giảm
    SellerVoucherDTO voucher("v_fail", "SALE10", "s_vouch_fail", 10.0, 50.0, "2030-12-31 23:59:59");
    std::vector<VoucherDTO*> vouchers = { &voucher };

    auto ck = BuyerBUS::finalOrder(true, *buyer, {"p_vouch_fail"}, vouchers, false);
    ASSERT_TRUE(ck.has_value());

    // Số dư phải là 100 - 30 = 70 (Voucher không được áp dụng)
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 70.0);

    EXPECT_TRUE(ProductDAO::remove("p_vouch_fail"));
}

// Test 3: Thanh toán sử dụng Coin và kiểm tra tích lũy Coin mới
TEST(BuyerBUSTest, CheckoutWithCoinsAndEarning) {
    auto seller = makeSeller("s_coin", "CoinSeller");
    auto prod = std::make_shared<ProductDTO>("p_coin", "CoinItem", 100.0, 10, seller);
    ASSERT_TRUE(ProductDAO::insert(prod));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "CoinUser", "cu@test.com", "pw123456", 200.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());

    // Giả sử người dùng đang có 50 xu
    buyer->setCoins(50.0);
    BuyerBUS::addToCart(*buyer, prod, 1);

    // Thanh toán có sử dụng xu (useCoins = true)
    auto ck = BuyerBUS::finalOrder(true, *buyer, {"p_coin"}, {}, true);
    ASSERT_TRUE(ck.has_value());

    // 1. Kiểm tra tiền mặt: 200 - (100 - 50 xu) = 150
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 150.0);

    // 2. Kiểm tra tích lũy xu mới:
    // Số tiền thực trả là 50.0. Tích lũy 0.1% -> 50.0 * 0.001 = 0.05 xu
    EXPECT_DOUBLE_EQ(buyer->getCoins(), 0.05);

    EXPECT_TRUE(ProductDAO::remove("p_coin"));
}

// Test 4: Kết hợp Voucher + Coin trong một đơn hàng
TEST(BuyerBUSTest, CheckoutCombinedVoucherAndCoins) {
    auto seller = makeSeller("s_comb", "CombinedSeller");
    auto prod = std::make_shared<ProductDTO>("p_comb", "CombinedItem", 1000.0, 10, seller);
    ASSERT_TRUE(ProductDAO::insert(prod));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "CombUser", "comb@test.com", "pw123456", 2000.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());

    buyer->setCoins(100.0);
    BuyerBUS::addToCart(*buyer, prod, 1);

    // Voucher giảm 10% (100.0)
    SellerVoucherDTO voucher("v_comb", "BIGSALE", "s_comb", 10.0, 500.0, "2030-12-31 23:59:59");
    std::vector<VoucherDTO*> vouchers = { &voucher };

    // Thực hiện thanh toán dùng cả 2
    auto ck = BuyerBUS::finalOrder(true, *buyer, {"p_comb"}, vouchers, true);
    ASSERT_TRUE(ck.has_value());

    // Tính toán:
    // Giá gốc: 1000.0
    // Sau voucher: 1000 - 100 = 900.0
    // Sau coin: 900 - 100 = 800.0 (Giá thực trả)
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 2000.0 - 800.0);
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 1200.0);

    // Tích lũy mới: 800.0 * 0.001 = 0.8 xu
    EXPECT_DOUBLE_EQ(buyer->getCoins(), 0.8);

    EXPECT_TRUE(ProductDAO::remove("p_comb"));
}

// Test 5: Voucher đạt đúng ngưỡng tối thiểu (Boundary Test)
TEST(BuyerBUSTest, VoucherExactlyAtMinOrder) {
    auto seller = makeSeller("s_bound", "BoundSeller");
    auto p = std::make_shared<ProductDTO>("p_bound", "Item", 50.0, 10, seller);
    ProductDAO::insert(p);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "BUser", "b@t.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, p, 1);

    SellerVoucherDTO v("v1", "MIN50", "s_bound", 10.0, 50.0, "2030-12-31 23:59:59");
    // Dùng finalOrder với tham số true để xác nhận thanh toán
    auto res = BuyerBUS::finalOrder(true, *buyer, {"p_bound"}, {&v}, false);
    ASSERT_TRUE(res.has_value());
    
    // 100 - (50 - 10%*50) = 55.0
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 55.0); 
    ProductDAO::remove("p_bound");
}

// Test 6: Nhiều sản phẩm cùng Shop cộng dồn để đủ điều kiện Voucher
TEST(BuyerBUSTest, MultiItemsSameShopVoucher) {
    auto seller = makeSeller("s_multi", "MultiSeller");
    auto p1 = std::make_shared<ProductDTO>("p1", "M1", 40.0, 10, seller);
    auto p2 = std::make_shared<ProductDTO>("p2", "M2", 40.0, 10, seller);
    ProductDAO::insert(p1); ProductDAO::insert(p2);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "MUser", "m@t.com", "pw123456", 200.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, p1, 1);
    BuyerBUS::addToCart(*buyer, p2, 1);

    SellerVoucherDTO v("v1", "MIN70", "s_multi", 10.0, 70.0, "2030-12-31 23:59:59");
    BuyerBUS::finalOrder(true, *buyer, {"p1", "p2"}, {&v}, false);
    
    // 200 - (80 - 8) = 128.0
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 128.0);
    ProductDAO::remove("p1"); ProductDAO::remove("p2");
}

// Test 7: Dùng xu vượt quá giá trị đơn hàng (Max Coin)
TEST(BuyerBUSTest, UseCoinsExceedingPrice) {
    auto seller = makeSeller("s_max", "MaxSeller");
    auto p = std::make_shared<ProductDTO>("p_max", "Item", 30.0, 10, seller);
    ProductDAO::insert(p);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "RUser", "r@t.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    buyer->setCoins(50.0); // Xu (50) > giá (30)
    
    BuyerBUS::addToCart(*buyer, p, 1);
    BuyerBUS::finalOrder(true, *buyer, {"p_max"}, {}, true);
    
    // Thực trả 0đ, balance giữ nguyên 100. Xu còn 50 - 30 = 20
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0);
    EXPECT_DOUBLE_EQ(buyer->getCoins(), 20.0);
    ProductDAO::remove("p_max");
}

// Test 8: Thanh toán một phần giỏ hàng nhưng không đủ điều kiện Voucher
TEST(BuyerBUSTest, PartialCheckoutVoucherFail) {
    auto seller = makeSeller("s_part", "PartSeller");
    auto p1 = std::make_shared<ProductDTO>("part1", "P1", 100.0, 10, seller);
    auto p2 = std::make_shared<ProductDTO>("part2", "P2", 100.0, 10, seller);
    ProductDAO::insert(p1); ProductDAO::insert(p2);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "PUser", "p@t.com", "pw123456", 500.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, p1, 1);
    BuyerBUS::addToCart(*buyer, p2, 1);

    SellerVoucherDTO v("v1", "MIN150", "s_part", 10.0, 150.0, "2030-12-31 23:59:59");
    // Chỉ chọn thanh toán p1 (100) -> Không đạt ngưỡng 150 của Voucher
    BuyerBUS::finalOrder(true, *buyer, {"part1"}, {&v}, false);
    
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 400.0); // Trả đủ 100đ
    ProductDAO::remove("part1"); ProductDAO::remove("part2");
}

// Test 9: Voucher của Shop A không được áp dụng cho sản phẩm Shop B
TEST(BuyerBUSTest, CrossShopVoucherInvalid) {
    auto sA = makeSeller("sA", "SellerA"); auto sB = makeSeller("sB", "SellerB");
    auto pB = std::make_shared<ProductDTO>("pB", "ItemB", 100.0, 10, sB);
    ProductDAO::insert(pB);

    auto bPack = UserBUS::registerUser(UserRole::BUYER, "CUser", "c@t.com", "pw123456", 200.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, pB, 1);

    SellerVoucherDTO vA("vA", "SHOP_A", "sA", 10.0, 50.0, "2030-12-31 23:59:59"); // Voucher của Shop A
    BuyerBUS::finalOrder(true, *buyer, {"pB"}, {&vA}, false);
    
    // Không giảm giá vì pB thuộc Shop B: 200 - 100 = 100.0
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0);
    ProductDAO::remove("pB");
}

// Test 10: Thanh toán với giỏ hàng trống
TEST(BuyerBUSTest, CheckoutWithEmptyCart) {
    auto bPack = UserBUS::registerUser(UserRole::BUYER, "EUser", "e@t.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());

    // Gọi finalOrder với giỏ hàng rỗng
    auto res = BuyerBUS::finalOrder(true, *buyer, {}, {}, false);
    ASSERT_FALSE(res.has_value());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0); // Số dư không đổi
}

// Test 11: Thanh toán với sản phẩm có tồn kho bằng 0
TEST(BuyerBUSTest, CheckoutWithOutOfStockProduct) {
    auto seller = makeSeller("s_oos", "OutOfStockSeller");
    auto p = std::make_shared<ProductDTO>("p_oos", "OOSItem", 100.0, 0, seller); // Stock = 0
    ProductDAO::insert(p);
    auto bPack = UserBUS::registerUser(UserRole::BUYER, "OOSUser", "o@t.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, p, 1);
    auto res = BuyerBUS::finalOrder(true, *buyer, {"p_oos"}, {}, false);
    ASSERT_FALSE(res.has_value());
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 100.0); // Số dư không đổi
    ProductDAO::remove("p_oos");    
}

// Test 12: Thanh toán với Voucher hết hạn (giả sử có trường expired trong VoucherDTO)
TEST(BuyerBUSTest, CheckoutWithExpiredVoucher) {
    auto seller = makeSeller("s_exp", "ExpiredVoucherSeller");
    auto p = std::make_shared<ProductDTO>("p_exp", "ExpItem", 100.0, 10, seller);
    ProductDAO::insert(p);
    auto bPack = UserBUS::registerUser(UserRole::BUYER, "ExpUser", "ase@t.com", "pw123456", 100.0);
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(bPack.value());
    BuyerBUS::addToCart(*buyer, p, 1);
    // Voucher này hết hạn vào năm 2020
    SellerVoucherDTO v("v_exp", "EXPIRED", "s_exp", 10.0, 50.0, "2020-01-01 00:00:00");
    // v.setExpired(true); // Giả sử có phương thức này
    auto res = BuyerBUS::finalOrder(true, *buyer, {"p_exp"}, {&v}, false);
    ASSERT_TRUE(res.has_value()); // Thanh toán vẫn thành công
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 0.0); // Không được giảm giá
    ProductDAO::remove("p_exp");    
}