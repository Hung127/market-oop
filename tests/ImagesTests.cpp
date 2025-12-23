#include <gtest/gtest.h>

#include "TestsCommon.h"

// Test ProductDTO with extra info and images
TEST(ProductDTO_ExtraInfo, SetAndGetExtraInfoWithImages) {
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
TEST(ImageHelper_ReadImage, ValidAndInvalidFiles) {
    // Test invalid file (non-existent)
    auto invalid = Utils::ImageHelper::readImageToBytes("nonexistent.jpg");
    EXPECT_TRUE(invalid.empty());

    // Test valid image file (assets/ copied to build/ by CMake)
    auto valid = Utils::ImageHelper::readImageToBytes("assets/cpu.jpg");
    EXPECT_FALSE(valid.empty());
    EXPECT_GT(valid.size(), 0);

    // Test isValidImage
    EXPECT_TRUE(Utils::ImageHelper::isValidImage("assets/cpu.jpg"));
    EXPECT_FALSE(Utils::ImageHelper::isValidImage("invalid.txt"));
}