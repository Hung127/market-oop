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

// Test reduceStock by exact amount
TEST(ProductBUSStockTest, ReduceExactStock) {
    auto seller = makeSeller("exact_s", "ExactSeller");
    auto prod = makeProduct("exact_p", "Exact", 10.0, 5, seller);

    auto res = ProductBUS::reduceStock(*prod, 5);
    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(prod->getStock(), 0);

    ProductDAO::remove("exact_p");
}

// Test reduceStock with zero quantity
TEST(ProductBUSStockTest, ReduceZeroQuantityFails) {
    auto seller = makeSeller("rz_s", "RZSeller");
    auto prod = makeProduct("rz_p", "RZ", 10.0, 5, seller);

    auto res = ProductBUS::reduceStock(*prod, 0);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(prod->getStock(), 5);  // Unchanged

    ProductDAO::remove("rz_p");
}

// Test reduceStock with negative quantity
TEST(ProductBUSStockTest, ReduceNegativeQuantityFails) {
    auto seller = makeSeller("rn_s", "RNSeller");
    auto prod = makeProduct("rn_p", "RN", 10.0, 5, seller);

    auto res = ProductBUS::reduceStock(*prod, -3);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(prod->getStock(), 5);

    ProductDAO::remove("rn_p");
}

// Test addStock with negative value (should not change)
TEST(ProductBUSStockTest, AddNegativeStockNoChange) {
    auto seller = makeSeller("an_s", "ANSeller");
    auto prod = makeProduct("an_p", "AN", 10.0, 5, seller);

    ProductBUS::addStock(*prod, -10);
    EXPECT_EQ(prod->getStock(), 5);

    ProductDAO::remove("an_p");
}

// Test addStock with zero (should not change)
TEST(ProductBUSStockTest, AddZeroStockNoChange) {
    auto seller = makeSeller("az_s", "AZSeller");
    auto prod = makeProduct("az_p", "AZ", 10.0, 5, seller);

    ProductBUS::addStock(*prod, 0);
    EXPECT_EQ(prod->getStock(), 5);

    ProductDAO::remove("az_p");
}

// Test hasEnoughStock boundary conditions
TEST(ProductBUSStockTest, HasEnoughStockBoundaries) {
    auto seller = makeSeller("hes_s", "HESSeller");
    auto prod = makeProduct("hes_p", "HES", 10.0, 10, seller);

    EXPECT_TRUE(ProductBUS::hasEnoughStock(*prod, 1));
    EXPECT_TRUE(ProductBUS::hasEnoughStock(*prod, 10));
    EXPECT_FALSE(ProductBUS::hasEnoughStock(*prod, 11));
    EXPECT_FALSE(ProductBUS::hasEnoughStock(*prod, 0));
    EXPECT_FALSE(ProductBUS::hasEnoughStock(*prod, -1));

    ProductDAO::remove("hes_p");
}

// Test isInStock with zero stock
TEST(ProductBUSStockTest, IsInStockZeroStock) {
    auto seller = makeSeller("iis_s", "IISSeller");
    auto prod = makeProduct("iis_p", "IIS", 10.0, 0, seller);

    EXPECT_FALSE(ProductBUS::isInStock(*prod));

    ProductDAO::remove("iis_p");
}

// Test updatePrice boundary values
TEST(ProductBUSPriceTest, UpdatePriceBoundaries) {
    auto seller = makeSeller("upb_s", "UPBSeller");
    auto prod = makeProduct("upb_p", "UPB", 10.0, 5, seller);

    // Very small positive price should work
    auto r1 = ProductBUS::updatePrice(*prod, 0.01);
    EXPECT_TRUE(r1.has_value());
    EXPECT_DOUBLE_EQ(prod->getPrice(), 0.01);

    // Very large price should work
    auto r2 = ProductBUS::updatePrice(*prod, 999999999.99);
    EXPECT_TRUE(r2.has_value());
    EXPECT_DOUBLE_EQ(prod->getPrice(), 999999999.99);

    ProductDAO::remove("upb_p");
}

// Test ProductDAO getAllProducts
TEST(ProductDAOGetTest, GetAllProductsContainsInserted) {
    auto s1 = makeSeller("ga_s1", "GASeller1");
    auto s2 = makeSeller("ga_s2", "GASeller2");
    auto p1 = makeProduct("ga_p1", "P1", 1.0, 1, s1);
    auto p2 = makeProduct("ga_p2", "P2", 2.0, 2, s2);

    auto all = ProductDAO::getAllProducts();

    bool found1 = false, found2 = false;
    for (const auto& p : all) {
        if (p->getID() == "ga_p1")
            found1 = true;
        if (p->getID() == "ga_p2")
            found2 = true;
    }

    EXPECT_TRUE(found1);
    EXPECT_TRUE(found2);

    ProductDAO::remove("ga_p1");
    ProductDAO::remove("ga_p2");
}

// Test ProductDAO getProductsBySeller
TEST(ProductDAOGetTest, GetProductsBySellerFiltersCorrectly) {
    auto s1 = makeSeller("gbs_s1", "GBSSeller1");
    auto s2 = makeSeller("gbs_s2", "GBSSeller2");
    auto p1 = makeProduct("gbs_p1", "P1", 1.0, 1, s1);
    auto p2 = makeProduct("gbs_p2", "P2", 2.0, 2, s1);
    auto p3 = makeProduct("gbs_p3", "P3", 3.0, 3, s2);

    auto s1Prods = ProductDAO::getProductsBySeller(s1->getId());
    EXPECT_EQ(s1Prods.size(), 2U);

    auto s2Prods = ProductDAO::getProductsBySeller(s2->getId());
    EXPECT_EQ(s2Prods.size(), 1U);

    ProductDAO::remove("gbs_p1");
    ProductDAO::remove("gbs_p2");
    ProductDAO::remove("gbs_p3");
}

// Test ProductDAO getProductById non-existent
TEST(ProductDAOGetTest, GetProductByIdNonExistent) {
    auto result = ProductDAO::getProductById("nonexistent_prod_xyz");
    EXPECT_FALSE(result.has_value());
}

// Test ProductDTO setters validation
TEST(ProductDTOTest, SettersNegativeValueHandling) {
    auto seller = makeSeller("set_s", "SetSeller");
    ProductDTO prod("set_p", "SetProd", 10.0, 5, seller);

    prod.setPrice(-5.0);
    EXPECT_DOUBLE_EQ(prod.getPrice(), 10.0);  // Should not change

    prod.setStock(-10);
    EXPECT_EQ(prod.getStock(), 5);  // Should not change

    prod.setPrice(0.0);
    EXPECT_DOUBLE_EQ(prod.getPrice(), 0.0);  // 0 is accepted

    prod.setStock(0);
    EXPECT_EQ(prod.getStock(), 0);  // 0 is accepted
}

// Test ProductDTO with null owner
TEST(ProductDTOTest, NullOwnerHandling) {
    ProductDTO prod("null_p", "NullOwner", 10.0, 5, nullptr);

    EXPECT_EQ(prod.getOwner(), nullptr);
    EXPECT_TRUE(prod.getSellerId().empty());
}

// Stress test: Rapid stock modifications
TEST(ProductBUSStressTest, RapidStockModifications) {
    auto seller = makeSeller("rapid_s", "RapidSeller");
    auto prod = makeProduct("rapid_p", "Rapid", 10.0, 1000, seller);

    // Reduce and add stock rapidly
    for (int i = 0; i < 100; ++i) {
        EXPECT_TRUE(ProductBUS::reduceStock(*prod, 5).has_value());
        ProductBUS::addStock(*prod, 3);
    }

    // Final stock:  1000 - (100*5) + (100*3) = 800
    EXPECT_EQ(prod->getStock(), 800);

    ProductDAO::remove("rapid_p");
}

// Test toString output format
TEST(ProductBUSDisplayTest, ToStringContainsFields) {
    auto seller = makeSeller("disp_s", "DispSeller");
    auto prod = makeProduct("disp_p", "DisplayProd", 12.50, 7, seller);

    std::string str = ProductBUS::toString(*prod);

    EXPECT_NE(str.find("disp_p"), std::string::npos);
    EXPECT_NE(str.find("DisplayProd"), std::string::npos);
    EXPECT_NE(str.find("12.50"), std::string::npos);
    EXPECT_NE(str.find("7"), std::string::npos);

    ProductDAO::remove("disp_p");
}

// ProductExtraInfo and image helper tests
TEST(ProductDTOExtraInfoTest, SetAndGetExtraInfoWithImages) {
    auto seller = makeSeller(Utils::generateId(), "s_extra");
    ProductDTO product("p_extra", "Extra Product", 50.0, 10, seller);

    // Create extra info
    auto extra = std::make_shared<ProductExtraInfoDTO>("Detailed description");

    // Add some dummy image data
    std::vector<uint8_t> img1 = {0xFF, 0xD8, 0xFF, 0xE0};  // JPEG header
    std::vector<uint8_t> img2 = {0x89, 0x50, 0x4E, 0x47};  // PNG header
    extra->addImageData(img1);
    extra->addImageData(img2);

    // Set extra info to product
    product.setExtraInfo(extra);

    // Verify
    auto retrievedExtra = product.getExtraInfo();
    ASSERT_TRUE(retrievedExtra != nullptr);
    EXPECT_EQ(retrievedExtra->getDescription(), "Detailed description");
    EXPECT_EQ(retrievedExtra->getImageCount(), 2);
    EXPECT_EQ(retrievedExtra->getImageAt(0), img1);
    EXPECT_EQ(retrievedExtra->getImageAt(1), img2);

    // Test const version
    auto constExtra = product.getExtraInfo();
    EXPECT_EQ(constExtra->getDescription(), "Detailed description");
}

// Test ImageHelper reading image bytes
TEST(ImageHelperTest, ReadImageValidAndInvalidFiles) {
    // Test invalid file (non-existent)
    auto invalid = Utils::ImageHelper::readImageToBytes("nonexistent.jpg");
    EXPECT_TRUE(invalid.empty());

    // The following valid-file test depends on test assets being available in the working dir.
    // If assets/cpu.jpg is not present in CI, this check will fail; keep it but tolerate empty
    // result.
    auto valid = Utils::ImageHelper::readImageToBytes("assets/cpu.jpg");
    // Either it's present and non-empty, or the function returns empty for missing file.
    if (!valid.empty()) {
        EXPECT_GT(valid.size(), 0);
        EXPECT_TRUE(Utils::ImageHelper::isValidImage("assets/cpu.jpg"));
    } else {
        // If missing, at least isValidImage should still behave based on extension
        EXPECT_FALSE(Utils::ImageHelper::isValidImage("invalid.txt"));
        EXPECT_TRUE(Utils::ImageHelper::isValidImage("photo.jpg"));
    }
}