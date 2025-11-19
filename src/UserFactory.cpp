#include "../include/UserFactory.h"

std::expected<std::unique_ptr<User>, std::string>
UserFactory::createUser(UserRole role, const std::string& id, const std::string& name,
                        const std::string& email, const std::string& password, double balance) {
    switch (role) {
        case UserRole::SELLER:
            return Seller::create(id, name, email, password);

        case UserRole::BUYER:
            return Buyer::create(id, name, email, password, balance);

        default:
            return std::unexpected("Unknown user role");
    }
}
