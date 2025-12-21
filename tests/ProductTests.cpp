#include <gtest/gtest.h>

#include "TestsCommon.h"

// PRODUCT BUS: price update and stock helpers
TEST(ProductBUSTest, UpdatePriceValidationAndStockHelpers) {
    auto seller = makeSeller("pbus_sp1", "PBus Seller");
    auto prod = makeProduct("pbus_p1", "ProdP", 12.5, 2, seller);

    // invalid price (zero/negative) rejected
    auto res0 = ProductBUS::updatePrice(*prod, 0.0);
    EXPECT_FALSE(res0.has_value());
    auto resNeg = ProductBUS::updatePrice(*prod, -5.0);
    EXPECT_FALSE(resNeg.has_value());

    // valid update
    auto resOk = ProductBUS::updatePrice(*prod, 15.0);
    EXPECT_TRUE(resOk.has_value());
    EXPECT_DOUBLE_EQ(prod->getPrice(), 15.0);

    // hasEnoughStock/isInStock
    EXPECT_TRUE(ProductBUS::hasEnoughStock(*prod, 1));
    EXPECT_FALSE(ProductBUS::hasEnoughStock(*prod, 100));
    EXPECT_TRUE(ProductBUS::isInStock(*prod));

    // addStock with negative should not change stock
    int before = prod->getStock();
    ProductBUS::addStock(*prod, -3);
    EXPECT_EQ(prod->getStock(), before);

    // addStock positive increases
    ProductBUS::addStock(*prod, 5);
    EXPECT_EQ(prod->getStock(), before + 5);

    EXPECT_TRUE(ProductDAO::remove(prod->getID()));
}

// PRODUCT DAO: insert duplicate, update, remove
TEST(ProductDAOTest, InsertDuplicateUpdateAndRemoveBehaviors) {
    auto seller = makeSeller("pdao_s1", "PDaoSeller");
    auto prod = makeProduct("pdao_p1", "PDaoProd", 3.5, 4, seller);

    // duplicate insert should fail
    auto dup = std::make_shared<ProductDTO>(*prod);
    EXPECT_FALSE(ProductDAO::insert(dup));  // duplicate id

    // update: modify price and stock and apply update
    prod->setPrice(4.5);
    prod->setStock(10);
    EXPECT_TRUE(ProductDAO::update(*prod));

    auto fetched = ProductDAO::getProductById(prod->getID());
    ASSERT_TRUE(fetched.has_value());
    EXPECT_DOUBLE_EQ(fetched.value()->getPrice(), 4.5);
    EXPECT_EQ(fetched.value()->getStock(), 10);

    // remove non-existent id -> false
    EXPECT_FALSE(ProductDAO::remove("nonexistent_prod_id"));

    EXPECT_TRUE(ProductDAO::remove(prod->getID()));
}
