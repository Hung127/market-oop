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
