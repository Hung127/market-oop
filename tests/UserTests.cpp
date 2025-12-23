#include <gtest/gtest.h>

#include "TestsCommon.h"

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
    auto seller = makeSeller("s_logout_seller", "Logout Seller");
    auto prod = makeProduct("p_logout", "P", 1.0, 10, seller);
    auto addRes = BuyerBUS::addToCart(*buyer, prod, 1);
    ASSERT_TRUE(addRes.has_value());
    EXPECT_GT(buyer->getCart().getItems().size(), 0U);

    // Perform logout (resets local shared_ptr)
    UserBUS::logout(u);
    EXPECT_EQ(u, nullptr);

    // cleanup product
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

// USER stress test
TEST(UserBUSStressTest, RegisterManyUniqueUsers) {
    const int N = 10;
    std::vector<std::shared_ptr<UserDTO>> created;
    created.reserve(N);
    for (int i = 0; i < N; ++i) {
        std::string unique = Utils::generateId();
        std::string email = "bulk+" + unique + "@example.com";
        auto reg =
            UserBUS::registerUser(UserRole::BUYER, "BulkUser", email, "strongPassword123", 1.0);
        ASSERT_TRUE(reg.has_value());
        created.push_back(reg.value());
    }

    // All created should be unique ids/emails
    std::unordered_set<std::string> ids;
    std::unordered_set<std::string> emails;
    for (auto& u : created) {
        ids.insert(u->getId());
        emails.insert(u->getEmail());
    }
    EXPECT_EQ(static_cast<int>(ids.size()), N);
    EXPECT_EQ(static_cast<int>(emails.size()), N);
}
