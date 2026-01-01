#include <gtest/gtest.h>

#include "TestsCommon.h"

// SELLER BUS: create product validation and updateStock failures
TEST(SellerBUSTest, CreateProductValidationAndUpdateStock) {
    auto seller = makeSeller("sell_s1", "SellerTest1");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    // invalid create params
    auto pBad = sbus.createProduct("", "N", 10.0, 1);
    EXPECT_FALSE(pBad.has_value());
    pBad = sbus.createProduct("id_ok", "", 10.0, 1);
    EXPECT_FALSE(pBad.has_value());
    pBad = sbus.createProduct("id_ok2", "N", -1.0, 1);
    EXPECT_FALSE(pBad.has_value());

    // create valid product
    auto pOkPack = sbus.createProduct("sell_p1", "Good", 5.0, 2);
    ASSERT_TRUE(pOkPack.has_value());
    auto pOk = pOkPack.value();
    EXPECT_EQ(pOk->getID(), "sell_p1");

    // updateStock negative invalid
    auto updBad = sbus.updateStock("sell_p1", -5);
    EXPECT_FALSE(updBad.has_value());

    // update non-existing product
    auto updNotFound = sbus.updateStock("no_such", 5);
    EXPECT_FALSE(updNotFound.has_value());

    // cleanup - product is stored in seller, remove from DAO if present
    ProductDAO::remove("sell_p1");
}

// Test createProduct with duplicate ID
TEST(SellerBUS_CreateProduct, DuplicateIdPrevention) {
    auto seller = makeSeller("dup_seller", "DupSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto p1 = sbus.createProduct("dup_p", "First", 10.0, 5);
    ASSERT_TRUE(p1.has_value());

    // Try to create another product with same ID
    auto p2 = sbus.createProduct("dup_p", "Second", 20.0, 10);
    EXPECT_FALSE(p2.has_value());

    ProductDAO::remove("dup_p");
}

// Test updateProduct with empty string and -1 (no-op)
TEST(SellerBUS_UpdateProduct, NoChangeParameters) {
    auto seller = makeSeller("upd_s", "UpdateSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto p = sbus.createProduct("upd_p", "Original", 15.0, 3);
    ASSERT_TRUE(p.has_value());

    // Try update with no changes (empty name, -1 price)
    auto upd = sbus.updateProduct("upd_p", "", -1);
    EXPECT_FALSE(upd.has_value());  // Should fail validation

    ProductDAO::remove("upd_p");
}

// Test updateProduct with zero price (should fail)
TEST(SellerBUS_UpdateProduct, ZeroPriceFails) {
    auto seller = makeSeller("zero_s", "ZeroSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto p = sbus.createProduct("zero_p", "Product", 10.0, 1);
    ASSERT_TRUE(p.has_value());

    auto upd = sbus.updateProduct("zero_p", "", 0.0);
    EXPECT_FALSE(upd.has_value());

    ProductDAO::remove("zero_p");
}

// Test updateProduct on non-existent product
TEST(SellerBUS_UpdateProduct, NonExistentProduct) {
    auto seller = makeSeller("nex_s", "NexSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto upd = sbus.updateProduct("nonexistent_prod", "NewName", 100.0);
    EXPECT_FALSE(upd.has_value());
}

// Test updateStock to zero (should succeed)
TEST(SellerBUS_UpdateStock, ZeroStockValid) {
    auto seller = makeSeller("zero_stock_s", "ZeroStockSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto p = sbus.createProduct("zs_p", "ZStock", 5.0, 10);
    ASSERT_TRUE(p.has_value());

    auto upd = sbus.updateStock("zs_p", 0);
    EXPECT_TRUE(upd.has_value());
    EXPECT_EQ(p.value()->getStock(), 0);

    ProductDAO::remove("zs_p");
}

// Test getMyProducts returns correct count
TEST(SellerBUS_GetProducts, CorrectCountAndContents) {
    auto seller = makeSeller("gp_s", "GetProdSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    // Create multiple products
    sbus.createProduct("gp_1", "P1", 1.0, 1);
    sbus.createProduct("gp_2", "P2", 2.0, 2);
    sbus.createProduct("gp_3", "P3", 3.0, 3);

    auto countPack = sbus.getProductCount();
    ASSERT_TRUE(countPack.has_value());
    EXPECT_EQ(countPack.value(), 3);

    auto prodsPack = sbus.getMyProducts();
    ASSERT_TRUE(prodsPack.has_value());
    EXPECT_EQ(prodsPack.value().size(), 3U);

    // Cleanup
    ProductDAO::remove("gp_1");
    ProductDAO::remove("gp_2");
    ProductDAO::remove("gp_3");
}

// Test searchMyProductsByName with exact match
TEST(SellerBUS_Search, ExactNameMatch) {
    auto seller = makeSeller("search_s", "SearchSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("sp1", "UniqueProductName", 10.0, 5);
    sbus.createProduct("sp2", "OtherProduct", 20.0, 3);

    auto results = sbus.searchMyProductsByName("UniqueProductName");
    EXPECT_EQ(results.size(), 1U);
    EXPECT_EQ(results[0]->getName(), "UniqueProductName");

    ProductDAO::remove("sp1");
    ProductDAO::remove("sp2");
}

// Test searchMyProductsByName case insensitive
TEST(SellerBUS_Search, CaseInsensitiveSearch) {
    auto seller = makeSeller("ci_s", "CaseSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("ci1", "TestProduct", 5.0, 1);

    auto r1 = sbus.searchMyProductsByName("testproduct");
    EXPECT_EQ(r1.size(), 1U);

    auto r2 = sbus.searchMyProductsByName("TESTPRODUCT");
    EXPECT_EQ(r2.size(), 1U);

    auto r3 = sbus.searchMyProductsByName("TeStPrOdUcT");
    EXPECT_EQ(r3.size(), 1U);

    ProductDAO::remove("ci1");
}

// Test searchMyProductsByName with empty keyword
TEST(SellerBUS_Search, EmptyKeywordReturnsEmpty) {
    auto seller = makeSeller("empty_s", "EmptySeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("e1", "Product", 1.0, 1);

    auto results = sbus.searchMyProductsByName("");
    EXPECT_TRUE(results.empty());

    ProductDAO::remove("e1");
}

// Test searchMyProductsByName partial match
TEST(SellerBUS_Search, PartialMatch) {
    auto seller = makeSeller("partial_s", "PartialSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("pm1", "Laptop", 500.0, 2);
    sbus.createProduct("pm2", "Desktop", 700.0, 1);
    sbus.createProduct("pm3", "Tablet", 300.0, 5);

    auto results = sbus.searchMyProductsByName("top");
    // Should match both "Laptop" and "Desktop" (substring match)
    EXPECT_GE(results.size(), 2U);

    ProductDAO::remove("pm1");
    ProductDAO::remove("pm2");
    ProductDAO::remove("pm3");
}

// Test searchMyClosestProduct fuzzy matching
TEST(SellerBUS_Search, ClosestProductFuzzyMatch) {
    auto seller = makeSeller("fuzzy_s", "FuzzySeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("fz1", "Laptop", 500.0, 1);
    sbus.createProduct("fz2", "Desktop", 600.0, 1);

    // Misspelled "Laptop" as "Loptap"
    auto closest = sbus.searchMyClosestProduct("Loptap");
    ASSERT_NE(closest, nullptr);
    EXPECT_EQ(closest->getName(), "Laptop");

    ProductDAO::remove("fz1");
    ProductDAO::remove("fz2");
}

// Test searchMyClosestProduct with very dissimilar keyword
TEST(SellerBUS_Search, ClosestProductNoGoodMatch) {
    auto seller = makeSeller("nogood_s", "NoGoodSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    sbus.createProduct("ng1", "Laptop", 500.0, 1);

    // Very different keyword
    auto closest = sbus.searchMyClosestProduct("XYZ123456");
    // Should return nullptr if similarity too low
    EXPECT_EQ(closest, nullptr);

    ProductDAO::remove("ng1");
}

// Test SellerBUS::create with null seller
TEST(SellerBUS_Create, NullSellerFails) {
    auto result = SellerBUS::create(nullptr);
    EXPECT_FALSE(result.has_value());
}

// Stress test: Create many products
TEST(SellerBUS_Stress, CreateManyProducts) {
    auto seller = makeSeller("stress_s", "StressSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    const int COUNT = 100;
    for (int i = 0; i < COUNT; ++i) {
        std::string id = "stress_p_" + std::to_string(i);
        auto p = sbus.createProduct(id, "Product" + std::to_string(i), 10.0 + i, i % 10);
        EXPECT_TRUE(p.has_value());
    }

    auto countPack = sbus.getProductCount();
    ASSERT_TRUE(countPack.has_value());
    EXPECT_EQ(countPack.value(), COUNT);

    // Cleanup
    for (int i = 0; i < COUNT; ++i) {
        ProductDAO::remove("stress_p_" + std::to_string(i));
    }
}

// Test getReceivedOrders returns empty for seller with no orders
TEST(SellerBUS_Orders, NoOrdersReturnsEmpty) {
    auto seller = makeSeller("noord_s", "NoOrderSeller");
    auto sbusPack = SellerBUS::create(seller);
    ASSERT_TRUE(sbusPack.has_value());
    SellerBUS sbus = sbusPack.value();

    auto ordersPack = sbus.getReceivedOrders();
    ASSERT_TRUE(ordersPack.has_value());
    EXPECT_TRUE(ordersPack.value().empty());
}

// Test getOrderItemsByStatus for all statuses
TEST(SellerBUS_Orders, GetItemsByAllStatuses) {
    auto seller = makeSeller("stat_s", "StatusSeller");
    auto product = makeProduct("stat_p", "StatusProd", 25.0, 10, seller);
    auto buyer = makeBuyer("StatusBuyer", 100.0);

    ASSERT_TRUE(BuyerBUS::addToCart(*buyer, product, 2).has_value());
    ASSERT_TRUE(BuyerBUS::checkout(*buyer).has_value());

    SellerBUS sbus = SellerBUS::create(seller).value();

    // Check all statuses
    for (auto status :
         {OrderItemStatus::PENDING, OrderItemStatus::CONFIRMED, OrderItemStatus::SHIPPED,
          OrderItemStatus::DELIVERED, OrderItemStatus::CANCELLED}) {
        auto itemsPack = sbus.getOrderItemsByStatus(status);
        EXPECT_TRUE(itemsPack.has_value());

        if (status == OrderItemStatus::PENDING) {
            EXPECT_GE(itemsPack.value().size(), 1U);
        }
    }

    ProductDAO::remove("stat_p");
}