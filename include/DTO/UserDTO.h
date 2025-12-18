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
    inline static std::vector<std::string> ids;

   public:
    virtual ~UserDTO() = default;

   public:
    UserDTO(const std::string& id, const std::string& name, const std::string& email,
            const std::string& hashedPassword);

    const static std::vector<std::string>& getIDS();

    std::string getId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPassword() const;

    virtual std::string getRole() const = 0;

    bool authenticate(const std::string& inputPass) const;
};

#endif  // USERDTO_H
