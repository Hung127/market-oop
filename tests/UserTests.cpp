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

// Test password hashing produces different hashes for same password (due to salt)
TEST(PasswordUtils_HashUniqueness, SamePasswordDifferentHashes) {
    std::string password = "mySecurePassword123";
    auto hash1 = Utils::PasswordUtils::hash(password);
    auto hash2 = Utils::PasswordUtils::hash(password);

    ASSERT_TRUE(hash1.has_value());
    ASSERT_TRUE(hash2.has_value());

    // Hashes should be different due to random salt
    EXPECT_NE(hash1.value(), hash2.value());

    // But both should verify correctly
    EXPECT_TRUE(Utils::PasswordUtils::verify(password, hash1.value()));
    EXPECT_TRUE(Utils::PasswordUtils::verify(password, hash2.value()));
}

// Test password verification fails for wrong passwords
TEST(PasswordUtils_Verify, WrongPasswordFails) {
    std::string correctPw = "correctPassword123";
    auto hashPack = Utils::PasswordUtils::hash(correctPw);
    ASSERT_TRUE(hashPack.has_value());

    EXPECT_FALSE(Utils::PasswordUtils::verify("wrongPassword", hashPack.value()));
    EXPECT_FALSE(Utils::PasswordUtils::verify("", hashPack.value()));
    EXPECT_FALSE(Utils::PasswordUtils::verify(correctPw + "x", hashPack.value()));
}

// Test empty password hashing
TEST(PasswordUtils_Hash, EmptyPasswordHandling) {
    auto result = Utils::PasswordUtils::hash("");
    ASSERT_TRUE(result.has_value());

    // Empty password should verify
    EXPECT_TRUE(Utils::PasswordUtils::verify("", result.value()));
    EXPECT_FALSE(Utils::PasswordUtils::verify("a", result.value()));
}

// Test verify with empty/invalid hash
TEST(PasswordUtils_Verify, EmptyHashReturnsFalse) {
    EXPECT_FALSE(Utils::PasswordUtils::verify("password", ""));
    EXPECT_FALSE(Utils::PasswordUtils::verify("password", "invalid_hash_format"));
}

// Test login with empty email/password
TEST(UserBUS_Login, EmptyCredentials) {
    auto res1 = UserBUS::login("", "password");
    EXPECT_FALSE(res1.has_value());

    auto res2 = UserBUS::login("test@test.com", "");
    EXPECT_FALSE(res2.has_value());

    auto res3 = UserBUS::login("", "");
    EXPECT_FALSE(res3.has_value());
}

// Test register with empty fields
TEST(UserBUS_Register, EmptyFieldsValidation) {
    std::string validPw = "validPassword123";

    // Empty name should still work (no validation on name currently)
    auto r1 = UserBUS::registerUser(UserRole::BUYER, "",
                                    "empty_name@" + Utils::generateId() + ".com", validPw, 0.0);
    EXPECT_TRUE(r1.has_value());  // Current implementation allows empty name

    // Empty email - should work but might cause issues (no email validation)
    auto r2 = UserBUS::registerUser(UserRole::BUYER, "Name", "", validPw, 0.0);
    EXPECT_TRUE(r2.has_value());  // Current implementation allows empty email
}

// Test register with exact minimum password length
TEST(UserBUS_Register, ExactMinimumPasswordLength) {
    std::string minPw(Utils::PasswordUtils::MIN_PASSWORD_LENGTH, 'a');
    auto r = UserBUS::registerUser(UserRole::BUYER, "MinPwUser",
                                   "minpw@" + Utils::generateId() + ".com", minPw, 0.0);
    EXPECT_TRUE(r.has_value());
}

// Test register with very long password
TEST(UserBUS_Register, VeryLongPassword) {
    std::string longPw(10000, 'x');
    auto r = UserBUS::registerUser(UserRole::BUYER, "LongPwUser",
                                   "longpw@" + Utils::generateId() + ".com", longPw, 100.0);
    EXPECT_TRUE(r.has_value());

    // Should be able to login with long password
    auto login = UserBUS::login(r.value()->getEmail(), longPw);
    EXPECT_TRUE(login.has_value());
}

// Test register with negative balance (should still work, no validation)
TEST(UserBUS_Register, NegativeBalanceAllowed) {
    auto r = UserBUS::registerUser(UserRole::BUYER, "NegBalUser",
                                   "negbal@" + Utils::generateId() + ".com", "password123", -500.0);
    ASSERT_TRUE(r.has_value());
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(r.value());
    ASSERT_NE(buyer, nullptr);
    EXPECT_DOUBLE_EQ(buyer->getBalance(), -500.0);
}

// Test getUserById with non-existent ID
TEST(UserDAO_GetById, NonExistentId) {
    auto result = UserDAO::getUserById("definitely_not_exists_id_12345");
    EXPECT_FALSE(result.has_value());
}

// Test casting between user types
TEST(UserDTO_RoleCasting, CastingBetweenTypes) {
    auto buyerPack =
        UserBUS::registerUser(UserRole::BUYER, "BuyerCast",
                              "buyercast@" + Utils::generateId() + ".com", "password123", 50.0);
    ASSERT_TRUE(buyerPack.has_value());

    auto sellerPack =
        UserBUS::registerUser(UserRole::SELLER, "SellerCast",
                              "sellercast@" + Utils::generateId() + ".com", "password123", 0.0);
    ASSERT_TRUE(sellerPack.has_value());

    // Buyer should cast to BuyerDTO but not SellerDTO
    auto buyer = std::dynamic_pointer_cast<BuyerDTO>(buyerPack.value());
    EXPECT_NE(buyer, nullptr);
    auto buyerAsSeller = std::dynamic_pointer_cast<SellerDTO>(buyerPack.value());
    EXPECT_EQ(buyerAsSeller, nullptr);

    // Seller should cast to SellerDTO but not BuyerDTO
    auto seller = std::dynamic_pointer_cast<SellerDTO>(sellerPack.value());
    EXPECT_NE(seller, nullptr);
    auto sellerAsBuyer = std::dynamic_pointer_cast<BuyerDTO>(sellerPack.value());
    EXPECT_EQ(sellerAsBuyer, nullptr);
}

// Test logout with null pointer (should not crash)
TEST(UserBUS_Logout, LogoutNullPointer) {
    std::shared_ptr<UserDTO> nullUser = nullptr;
    EXPECT_NO_THROW(UserBUS::logout(nullUser));
    EXPECT_EQ(nullUser, nullptr);
}

// Test rapid successive logins
TEST(UserBUS_Login, RapidSuccessiveLogins) {
    std::string unique = Utils::generateId();
    std::string email = "rapid@" + unique + ".com";
    std::string password = "rapidPassword123";

    auto reg = UserBUS::registerUser(UserRole::BUYER, "RapidUser", email, password, 10.0);
    ASSERT_TRUE(reg.has_value());

    // Login multiple times in succession
    for (int i = 0; i < 10; ++i) {
        auto login = UserBUS::login(email, password);
        EXPECT_TRUE(login.has_value());
    }
}

// Stress test: Create users with very similar emails
TEST(UserRegistration_SimilarEmails, UniqueEmailsWithSimilarPatterns) {
    std::string base = "similar_" + Utils::generateId();
    std::vector<std::string> emails = {base + "@test.com", base + "1@test.com", base + "2@test.com",
                                       base + "@test1.com", base + "@test2.com"};

    for (const auto& email : emails) {
        auto r = UserBUS::registerUser(UserRole::BUYER, "User", email, "password123", 0.0);
        EXPECT_TRUE(r.has_value()) << "Failed for email: " << email;
    }
}

// Test ID generation uniqueness
TEST(Utils_GenerateId, GeneratesUniqueIds) {
    std::unordered_set<std::string> ids;
    const int COUNT = 1000;

    for (int i = 0; i < COUNT; ++i) {
        std::string id = Utils::generateId();
        EXPECT_FALSE(id.empty());
        EXPECT_TRUE(ids.insert(id).second) << "Duplicate ID generated: " << id;
    }

    EXPECT_EQ(static_cast<int>(ids.size()), COUNT);
}

// Test concurrent-like ID generation (thread_local RNG test)
TEST(Utils_GenerateId, QuickSuccessiveGeneration) {
    std::vector<std::string> ids;
    ids.reserve(100);

    for (int i = 0; i < 100; ++i) {
        ids.push_back(Utils::generateId());
    }

    std::unordered_set<std::string> uniqueIds(ids.begin(), ids.end());
    EXPECT_EQ(ids.size(), uniqueIds.size());
}