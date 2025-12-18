#include "../../include/DAO/UserDAO.h"

#include <expected>
#include <memory>
#include <string>

#include "../../include/DTO/UserDTO.h"

std::expected<std::shared_ptr<UserDTO>, std::string>
UserDAO::getUserByEmail(const std::string& email) {
    return nullptr;
}
