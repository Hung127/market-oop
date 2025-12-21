#include <gtest/gtest.h>

#include "TestsCommon.h"

// CART BUS: null product, invalid quantity, recalc removing expired items, reduce/remove edge cases
TEST(CartBUSTest, AddNullOrInvalidQuantityAndRecalculateExpiredItems) {
    CartDTO cart;

    // add nullptr should fail
    auto addNull = CartBUS::add(cart, nullptr, 1);
    EXPECT_FALSE(addNull.has_value());

    // create product and add then reset product pointer to simulate expiration
    std::shared_ptr<ProductDTO> prod;
    {
        auto seller = makeSeller("cart_s1", "CartSeller1");
        prod = makeProduct("cart_p1", "Temp", 2.0, 5, seller);
    }  // seller still exists but prod is in DAO and also held by 'prod' shared_ptr

    ASSERT_TRUE(CartBUS::add(cart, prod, 2).has_value());
    EXPECT_EQ(cart.getItems().size(), 1U);

    // simulate product going out of scope by resetting the shared_ptr and removing from DAO
    prod.reset();
    ProductDAO::remove("cart_p1");

    // recalculate should remove the expired item
    auto recRes = CartBUS::recalculateTotal(cart);
    EXPECT_TRUE(recRes.has_value());
    EXPECT_EQ(cart.getItems().size(), 0U);
    EXPECT_DOUBLE_EQ(cart.getTotalPrice(), 0.0);
}

TEST(CartBUSTest, RemoveAndReduceQuantityEdgeCases) {
    auto seller = makeSeller("cart_s2", "CartSeller2");
    auto prod = makeProduct("cart_p2", "RemoveReduce", 1.0, 5, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 3).has_value());
    // remove nonexistent
    EXPECT_FALSE(CartBUS::removeItem(cart, "no_such"));

    // reduce more than present -> should remove item
    ASSERT_TRUE(CartBUS::reduceQuantity(cart, "cart_p2", 5).has_value());
    EXPECT_EQ(cart.getItems().size(), 0U);

    EXPECT_TRUE(ProductDAO::remove(prod->getID()));
}

TEST(CartBUSTest, ReduceQuantityInvalidValues) {
    auto seller = makeSeller("cart_s3", "CartSeller3");
    auto prod = makeProduct("cart_p3", "QtyInvalid", 2.0, 3, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 2).has_value());

    auto bad = CartBUS::reduceQuantity(cart, "cart_p3", 0);
    EXPECT_FALSE(bad.has_value());
    auto badNeg = CartBUS::reduceQuantity(cart, "cart_p3", -1);
    EXPECT_FALSE(badNeg.has_value());

    EXPECT_TRUE(ProductDAO::remove(prod->getID()));
}
