#include <unordered_set>

#include <gtest/gtest.h>

#include "../include/BUS/BuyerBUS.h"
#include "../include/BUS/CartBUS.h"
#include "../include/BUS/ProductBUS.h"
#include "../include/DAO/ProductDAO.h"
#include "../include/DTO/BuyerDTO.h"
#include "../include/DTO/ProductDTO.h"
#include "../include/DTO/PurchaseHistoryDTO.h"
#include "../include/DTO/SellerDTO.h"
#include "../include/Utils/Utils.h"

// Helper to create a seller
static std::shared_ptr<SellerDTO> makeSeller(const std::string& id = "s_test",
                                             const std::string& name = "Seller",
                                             const std::string& email = "s@test",
                                             const std::string& pwd = "pw") {
    return std::make_shared<SellerDTO>(id, name, email, pwd);
}

TEST(ProductBUSTest, ReduceStockSuccessAndFailure) {
    auto seller = makeSeller("s_stock");
    ProductDTO p("p_stock", "StockItem", 5.0, 10, seller);
    // insert into mock DAO
    ASSERT_TRUE(ProductDAO::insert(p));

    // reduce valid amount
    auto reduce_ok = ProductBUS::reduceStock(p, 3);
    EXPECT_TRUE(reduce_ok.has_value());
    EXPECT_EQ(p.getStock(), 7);

    // reduce too much
    auto reduce_bad = ProductBUS::reduceStock(p, 100);
    EXPECT_FALSE(reduce_bad.has_value());

    // cleanup
    EXPECT_TRUE(ProductDAO::remove("p_stock"));
}

TEST(CartBUSTest, AddAndGetTotal) {
    auto seller = makeSeller("s_cart");
    auto prod = std::make_shared<ProductDTO>("p_cart", "CartItem", 2.5, 5, seller);
    ASSERT_TRUE(ProductDAO::insert(*prod));

    CartDTO cart;
    auto addRes = CartBUS::add(cart, prod, 3);
    EXPECT_TRUE(addRes.has_value());

    auto totalPack = CartBUS::getTotal(cart);
    ASSERT_TRUE(totalPack.has_value());
    EXPECT_DOUBLE_EQ(totalPack.value(), 2.5 * 3);

    // reduce quantity
    auto reduceRes = CartBUS::reduceQuantity(cart, "p_cart", 2);
    EXPECT_TRUE(reduceRes.has_value());
    auto totalPack2 = CartBUS::getTotal(cart);
    ASSERT_TRUE(totalPack2.has_value());
    EXPECT_DOUBLE_EQ(totalPack2.value(), 2.5 * 1);

    // cleanup
    EXPECT_TRUE(ProductDAO::remove("p_cart"));
}

TEST(BuyerBUSTest, CheckoutSuccessAndHistory) {
    // setup seller and product
    auto seller = makeSeller("s_buyer1");
    auto prod_ptr = std::make_shared<ProductDTO>("p_buy1", "BuyItem", 10.0, 5, seller);
    ASSERT_TRUE(ProductDAO::insert(prod_ptr));

    // create buyer with enough balance
    auto buyerPack = BuyerBUS::create("b_test1", "TestBuyer", "tb@example.com", "pw", 100.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = buyerPack.value();

    // add to cart and checkout
    auto a1 = BuyerBUS::addToCart(*buyer, prod_ptr, 2);
    ASSERT_TRUE(a1.has_value());

    auto ck = BuyerBUS::checkout(*buyer);
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

// Insufficient balance test
TEST(BuyerBUSTest, CheckoutInsufficientBalance) {
    auto seller = makeSeller("s_buyer2");
    auto prod_ptr = std::make_shared<ProductDTO>("p_buy2", "Expensive", 50.0, 3, seller);
    ASSERT_TRUE(ProductDAO::insert(*prod_ptr));

    auto buyerPack = BuyerBUS::create("b_test2", "PoorBuyer", "poor@example.com", "pw", 10.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = buyerPack.value();

    auto addRes = BuyerBUS::addToCart(*buyer, prod_ptr, 1);
    ASSERT_TRUE(addRes.has_value());

    auto ck = BuyerBUS::checkout(*buyer);
    EXPECT_FALSE(ck.has_value());  // should fail

    // ensure nothing changed: balance unchanged, stock unchanged
    EXPECT_DOUBLE_EQ(buyer->getBalance(), 10.0);
    auto fetched = ProductDAO::getProductById("p_buy2");
    ASSERT_TRUE(fetched.has_value());
    EXPECT_EQ(fetched.value()->getStock(), 3);

    // cleanup
    EXPECT_TRUE(ProductDAO::remove("p_buy2"));
}

// Test password hashing & verification using PasswordUtils
TEST(Security_PasswordUtils, HashAndVerify) {
    using Utils::PasswordUtils;

    const std::string plain = "super-secret-password-123!";

    auto hashedPack = PasswordUtils::hash(plain);
    ASSERT_TRUE(hashedPack.has_value()) << "Hash failed: " << hashedPack.error();
    std::string stored = hashedPack.value();

    // verify correct password
    EXPECT_TRUE(PasswordUtils::verify(plain, stored));

    // incorrect password fails
    EXPECT_FALSE(PasswordUtils::verify("wrong-password", stored));

    // hashing same password twice should produce different encodings (due to random salt)
    auto hashedPack2 = PasswordUtils::hash(plain);
    ASSERT_TRUE(hashedPack2.has_value());
    std::string stored2 = hashedPack2.value();
    EXPECT_NE(stored, stored2);
    // but both verify successfully
    EXPECT_TRUE(PasswordUtils::verify(plain, stored2));
}

// Test generateId uniqueness (basic)
TEST(Utils_IdGenerator, GenerateUniqueIds) {
    const int N = 1000;
    std::unordered_set<std::string> seen;
    seen.reserve(N * 2);

    for (int i = 0; i < N; ++i) {
        std::string id = Utils::generateId();
        // Ensure non-empty and unique
        ASSERT_FALSE(id.empty());
        auto [it, inserted] = seen.emplace(std::move(id));
        EXPECT_TRUE(inserted) << "Duplicate id generated at iteration " << i;
    }

    EXPECT_EQ(static_cast<int>(seen.size()), N);
}

// Test UserFactory creating both roles and fields correctness
TEST(UserFactory_CreateUser, SellerAndBuyer) {
    using namespace std::string_literals;

    auto sellerPack = UserFactory::createUser(UserRole::SELLER, "s_test_id", "SellerName",
                                              "s@example.com", "hashed_pw", 0.0);
    ASSERT_TRUE(sellerPack.has_value());
    std::shared_ptr<UserDTO> seller = sellerPack.value();
    ASSERT_NE(seller, nullptr);
    EXPECT_EQ(seller->getHashedPassword(), "hashed_pw");
    EXPECT_EQ(seller->getRole(), "SELLER");

    auto buyerPack = UserFactory::createUser(UserRole::BUYER, "b_test_id", "BuyerName",
                                             "b@example.com", "hashed_pw", 50.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<UserDTO> buyer = buyerPack.value();
    ASSERT_NE(buyer, nullptr);
    EXPECT_EQ(buyer->getHashedPassword(), "hashed_pw");
    EXPECT_EQ(buyer->getRole(), "BUYER");
}

// Integration test: register a user via UserBUS and login succeeds
TEST(UserBUS_RegisterAndLogin, RegisterThenLogin) {
    // Use unique values to avoid collision with other tests
    std::string unique = Utils::generateId();
    std::string email = "test+" + unique + "@example.com";
    std::string name = "TUser_" + unique;
    std::string password = "password-For-Test-!";

    // Register user
    auto regPack = UserBUS::registerUser(UserRole::BUYER, name, email, password, 20.0);
    ASSERT_TRUE(regPack.has_value()) << "Register failed: " << regPack.error();
    std::shared_ptr<UserDTO> created = regPack.value();
    ASSERT_NE(created, nullptr);

    // The returned user's stored hashed password should not equal plaintext
    EXPECT_NE(created->getHashedPassword(), password);

    // Try login with correct credentials
    auto loginPack = UserBUS::login(email, password);
    ASSERT_TRUE(loginPack.has_value()) << "Login failed: " << loginPack.error();
    std::shared_ptr<UserDTO> logged = loginPack.value();
    ASSERT_NE(logged, nullptr);
    EXPECT_EQ(logged->getEmail(), email);

    // Try login with wrong password
    auto loginBad = UserBUS::login(email, "wrong-" + password);
    EXPECT_FALSE(loginBad.has_value());
}
