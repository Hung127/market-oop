#ifndef USERDTO_H
#define USERDTO_H

#include <string>
#include <vector>

class UserDTO {
   protected:
    std::string _id;
    std::string _name;
    std::string _email;
    std::string _hashedPassword;

   public:
    virtual ~UserDTO() = default;

   public:
    UserDTO(const std::string& id, const std::string& name, const std::string& email,
            const std::string& hashedPassword);

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getEmail() const;
    const std::string& getHashedPassword() const;

    virtual std::string getRole() const = 0;
};

#endif  // USERDTO_H
