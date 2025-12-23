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

// Test add product twice (should increment quantity)
TEST(CartBUS_Add, AddSameProductTwiceIncrementsQuantity) {
    auto seller = makeSeller("inc_s", "IncSeller");
    auto prod = makeProduct("inc_p", "IncProd", 5.0, 10, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 2).has_value());
    ASSERT_TRUE(CartBUS::add(cart, prod, 3).has_value());

    EXPECT_EQ(cart.getItems().size(), 1U);              // Still one entry
    EXPECT_EQ(CartBUS::getQuantity(cart, "inc_p"), 5);  // 2 + 3

    ProductDAO::remove("inc_p");
}

// Test add with very large quantity
TEST(CartBUS_Add, VeryLargeQuantity) {
    auto seller = makeSeller("vlq_s", "VLQSeller");
    auto prod = makeProduct("vlq_p", "VLQ", 1.0, 1000000, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 999999).has_value());

    EXPECT_EQ(CartBUS::getQuantity(cart, "vlq_p"), 999999);

    ProductDAO::remove("vlq_p");
}

// Test add with zero quantity (should fail)
TEST(CartBUS_Add, ZeroQuantityFails) {
    auto seller = makeSeller("zq_s", "ZQSeller");
    auto prod = makeProduct("zq_p", "ZQ", 1.0, 5, seller);

    CartDTO cart;
    auto res = CartBUS::add(cart, prod, 0);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(cart.getItems().size(), 0U);

    ProductDAO::remove("zq_p");
}

// Test add with negative quantity (should fail)
TEST(CartBUS_Add, NegativeQuantityFails) {
    auto seller = makeSeller("nq_s", "NQSeller");
    auto prod = makeProduct("nq_p", "NQ", 1.0, 5, seller);

    CartDTO cart;
    auto res = CartBUS::add(cart, prod, -5);
    EXPECT_FALSE(res.has_value());

    ProductDAO::remove("nq_p");
}

// Test removeItem on empty cart
TEST(CartBUS_Remove, RemoveFromEmptyCart) {
    CartDTO cart;
    EXPECT_FALSE(CartBUS::removeItem(cart, "any_id"));
}

// Test reduceQuantity to zero removes item
TEST(CartBUS_Reduce, ReduceToZeroRemovesItem) {
    auto seller = makeSeller("rtz_s", "RTZSeller");
    auto prod = makeProduct("rtz_p", "RTZ", 5.0, 10, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 5).has_value());
    ASSERT_TRUE(CartBUS::reduceQuantity(cart, "rtz_p", 5).has_value());

    EXPECT_EQ(cart.getItems().size(), 0U);

    ProductDAO::remove("rtz_p");
}

// Test reduceQuantity on non-existent product
TEST(CartBUS_Reduce, NonExistentProductReturnsError) {
    CartDTO cart;
    auto res = CartBUS::reduceQuantity(cart, "nonexistent", 1);
    EXPECT_FALSE(res.has_value());
}

// Test hasProduct on empty cart
TEST(CartBUS_Has, EmptyCartReturnsFalse) {
    CartDTO cart;
    EXPECT_FALSE(CartBUS::hasProduct(cart, "any_id"));
}

// Test getQuantity on empty cart
TEST(CartBUS_GetQuantity, EmptyCartReturnsZero) {
    CartDTO cart;
    EXPECT_EQ(CartBUS::getQuantity(cart, "any_id"), 0);
}

// Test clear on already empty cart
TEST(CartBUS_Clear, ClearEmptyCart) {
    CartDTO cart;
    EXPECT_NO_THROW(CartBUS::clear(cart));
    EXPECT_EQ(cart.getItems().size(), 0U);
    EXPECT_DOUBLE_EQ(cart.getTotalPrice(), 0.0);
}

// Test getTotal with multiple items
TEST(CartBUS_Total, MultipleItemsCalculation) {
    auto seller = makeSeller("multi_s", "MultiSeller");
    auto p1 = makeProduct("multi_p1", "P1", 10.0, 5, seller);
    auto p2 = makeProduct("multi_p2", "P2", 20.0, 3, seller);
    auto p3 = makeProduct("multi_p3", "P3", 5.5, 10, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, p1, 2).has_value());  // 20
    ASSERT_TRUE(CartBUS::add(cart, p2, 1).has_value());  // 20
    ASSERT_TRUE(CartBUS::add(cart, p3, 4).has_value());  // 22

    auto totalPack = CartBUS::getTotal(cart);
    ASSERT_TRUE(totalPack.has_value());
    EXPECT_DOUBLE_EQ(totalPack.value(), 62.0);

    ProductDAO::remove("multi_p1");
    ProductDAO::remove("multi_p2");
    ProductDAO::remove("multi_p3");
}

// Test recalculateTotal handles price changes
TEST(CartBUS_Recalc, HandlesProductPriceChanges) {
    auto seller = makeSeller("price_s", "PriceSeller");
    auto prod = makeProduct("price_p", "PriceChange", 10.0, 5, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 3).has_value());
    EXPECT_DOUBLE_EQ(cart.getTotalPrice(), 30.0);

    // Change product price
    prod->setPrice(15.0);

    // Recalculate should reflect new price
    ASSERT_TRUE(CartBUS::recalculateTotal(cart).has_value());
    EXPECT_DOUBLE_EQ(cart.getTotalPrice(), 45.0);

    ProductDAO::remove("price_p");
}

// Stress test: Add many different products
TEST(CartBUS_Stress, AddManyDifferentProducts) {
    auto seller = makeSeller("many_s", "ManySeller");
    CartDTO cart;

    const int COUNT = 50;
    for (int i = 0; i < COUNT; ++i) {
        std::string id = "many_p_" + std::to_string(i);
        auto prod = makeProduct(id, "Product" + std::to_string(i), 10.0, 100, seller);
        ASSERT_TRUE(CartBUS::add(cart, prod, 1).has_value());
    }

    EXPECT_EQ(cart.getItems().size(), static_cast<size_t>(COUNT));

    auto totalPack = CartBUS::getTotal(cart);
    ASSERT_TRUE(totalPack.has_value());
    EXPECT_DOUBLE_EQ(totalPack.value(), 10.0 * COUNT);

    // Cleanup
    for (int i = 0; i < COUNT; ++i) {
        ProductDAO::remove("many_p_" + std::to_string(i));
    }
}

// Test displayCart doesn't crash on empty cart
TEST(CartBUS_Display, DisplayEmptyCart) {
    CartDTO cart;
    EXPECT_NO_THROW(CartBUS::displayCart(cart));
}

// Test displayCart with items
TEST(CartBUS_Display, DisplayCartWithItems) {
    auto seller = makeSeller("disp_cart_s", "DispCartSeller");
    auto prod = makeProduct("disp_cart_p", "DispCart", 5.0, 10, seller);

    CartDTO cart;
    ASSERT_TRUE(CartBUS::add(cart, prod, 2).has_value());

    EXPECT_NO_THROW(CartBUS::displayCart(cart));

    ProductDAO::remove("disp_cart_p");
}