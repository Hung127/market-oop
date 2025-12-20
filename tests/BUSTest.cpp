#include <memory>
#include <unordered_set>

#include <gtest/gtest.h>

#include "../include/BUS/BuyerBUS.h"
#include "../include/BUS/CartBUS.h"
#include "../include/BUS/ProductBUS.h"
#include "../include/BUS/UserBUS.h"
#include "../include/DAO/ProductDAO.h"
#include "../include/DAO/UserDAO.h"
#include "../include/DTO/BuyerDTO.h"
#include "../include/DTO/ProductDTO.h"
#include "../include/DTO/PurchaseHistoryDTO.h"
#include "../include/DTO/SellerDTO.h"
#include "../include/UserFactory.h"
#include "../include/Utils/Utils.h"

// Helper to create a seller
static std::shared_ptr<SellerDTO> makeSeller(const std::string& id = "s_test",
                                             const std::string& name = "Seller",
                                             const std::string& email = "s@test",
                                             const std::string& pwd = "pw") {
    return std::make_shared<SellerDTO>(id, name, email, pwd);
}

// ----------------------------- Existing product/cart/buyer tests -----------------------------

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
    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "TestBuyer", "tb@example.com",
                                           "pwdjsfaklpjf", 100.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    ASSERT_NE(buyer, nullptr);

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

TEST(BuyerBUSTest, CheckoutInsufficientBalance) {
    auto seller = makeSeller("s_buyer2");
    auto prod_ptr = std::make_shared<ProductDTO>("p_buy2", "Expensive", 50.0, 3, seller);
    ASSERT_TRUE(ProductDAO::insert(*prod_ptr));

    auto buyerPack = UserBUS::registerUser(UserRole::BUYER, "PoorBuyer", "poor@example.com",
                                           "pwsdajkflasdjklfjl", 10.0);
    ASSERT_TRUE(buyerPack.has_value());
    std::shared_ptr<BuyerDTO> buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    ASSERT_NE(buyer, nullptr);

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

// ----------------------------- Security / utils tests -----------------------------

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

// ----------------------------- Additional use-case tests -----------------------------

// Check registering with duplicate email fails
TEST(UserRegistration_DuplicateEmail, PreventDuplicateEmail) {
    std::string unique = Utils::generateId();
    std::string email = "dup+" + unique + "@example.com";

    // First registration should succeed
    auto r1 = UserBUS::registerUser(UserRole::BUYER, "User1", email, "strong-pass-123", 0.0);
    ASSERT_TRUE(r1.has_value()) << "First registration failed: " << r1.error();

    // Second registration with same email should fail
    auto r2 = UserBUS::registerUser(UserRole::BUYER, "User2", email, "another-pass-456", 0.0);
    EXPECT_FALSE(r2.has_value());
}

// Check registering fails for short password
TEST(UserBUS_Register_PasswordTooShort, RejectShortPassword) {
    std::string unique = Utils::generateId();
    std::string email = "short+" + unique + "@example.com";
    std::string shortPw = std::string(Utils::PasswordUtils::MIN_PASSWORD_LENGTH - 1, 'a');

    auto r = UserBUS::registerUser(UserRole::BUYER, "ShortPwUser", email, shortPw, 0.0);
    EXPECT_FALSE(r.has_value());
}

// Verify UserDAO existByEmail and existById reflect registrations
TEST(UserDAO_ExistChecks, ExistByEmailAndId) {
    std::string unique = Utils::generateId();
    std::string email = "exist+" + unique + "@example.com";

    auto r = UserBUS::registerUser(UserRole::BUYER, "ExistUser", email, "valid-pass-123", 5.0);
    ASSERT_TRUE(r.has_value());
    std::shared_ptr<UserDTO> u = r.value();
    ASSERT_NE(u, nullptr);

    EXPECT_TRUE(UserDAO::existByEmail(email));
    EXPECT_TRUE(UserDAO::existById(u->getId()));
}

// Test UserDAO::getUserByEmail returns correct user after register
TEST(UserDAO_GetByEmail, ReturnsRegisteredUser) {
    std::string unique = Utils::generateId();
    std::string email = "getby+" + unique + "@example.com";
    auto reg = UserBUS::registerUser(UserRole::SELLER, "SellerForGet", email, "sellerpw-1", 0.0);
    ASSERT_TRUE(reg.has_value());
    std::shared_ptr<UserDTO> created = reg.value();
    ASSERT_NE(created, nullptr);

    auto fetchedPack = UserDAO::getUserByEmail(email);
    ASSERT_TRUE(fetchedPack.has_value());
    auto fetched = fetchedPack.value();
    ASSERT_NE(fetched, nullptr);
    EXPECT_EQ(fetched->getEmail(), email);
    EXPECT_EQ(fetched->getId(), created->getId());
}

// Test logout clears the local shared_ptr
TEST(UserBUS_Logout, ClearsSharedPtr) {
    std::string unique = Utils::generateId();
    std::string email = "logout+" + unique + "@example.com";
    auto reg = UserBUS::registerUser(UserRole::BUYER, "LogoutUser", email, "logout-pass-1", 10.0);
    ASSERT_TRUE(reg.has_value());
    std::shared_ptr<UserDTO> u = reg.value();
    ASSERT_NE(u, nullptr);

    // Cast to buyer to demonstrate we can still access buyer-specific data
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(u);
    ASSERT_NE(buyer, nullptr);

    // Add something to cart to check it's not implicitly cleared by logout (our simple logout
    // resets pointer only)
    auto seller = makeSeller("s_logout_seller");
    auto prod = std::make_shared<ProductDTO>("p_logout", "P", 1.0, 10, seller);
    ASSERT_TRUE(ProductDAO::insert(prod));
    auto addRes = BuyerBUS::addToCart(*buyer, prod, 1);
    ASSERT_TRUE(addRes.has_value());
    EXPECT_GT(buyer->getCart().getItems().size(), 0U);

    // Perform logout (resets local shared_ptr)
    UserBUS::logout(u);
    EXPECT_EQ(u, nullptr);

    // The buyer object might still be alive if DAO or other owners keep it; but local reference was
    // cleared. cleanup product
    EXPECT_TRUE(ProductDAO::remove("p_logout"));
}

// Test direct UserDAO duplicate id prevention (using UserFactory + UserDAO::addUser)
TEST(UserDAO_DuplicateId, PreventDuplicateId) {
    std::string id = Utils::generateId();
    auto u1pack = UserFactory::createUser(UserRole::BUYER, id, "U1",
                                          "u1+" + Utils::generateId() + "@ex.com", "hp1", 0.0);
    ASSERT_TRUE(u1pack.has_value());
    auto u1 = u1pack.value();
    ASSERT_TRUE(UserDAO::addUser(u1).has_value());

    // Create second user with same id but different email -> add should fail
    auto u2pack = UserFactory::createUser(UserRole::SELLER, id, "U2",
                                          "u2+" + Utils::generateId() + "@ex.com", "hp2", 0.0);
    ASSERT_TRUE(u2pack.has_value());
    auto u2 = u2pack.value();
    auto add2 = UserDAO::addUser(u2);
    EXPECT_FALSE(add2.has_value());
}

// Test login failure for non-existent email
TEST(UserBUS_Login_NonexistentEmail, FailsGracefully) {
    std::string fakeEmail = "no-user-" + Utils::generateId() + "@example.com";
    auto res = UserBUS::login(fakeEmail, "whatever");
    EXPECT_FALSE(res.has_value());
}
