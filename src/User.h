#ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    std::string username;
    std::string password;
    std::string role; // "Admin" 或 "Regular"

public:
    User(const std::string& username, const std::string& password, const std::string& role);
    virtual ~User() = default;

    std::string getUsername() const { return username; }
    std::string getRole() const { return role; }
    bool verifyPassword(const std::string& pwd) const { return password == pwd; }
    std::string getPassword() const { return password; } // 用於資料寫檔

    // 權限管制多型方法
    virtual bool canManageInventory() const { return false; }
    virtual bool canManageUsers() const { return false; }
};

class AdminUser : public User {
public:
    AdminUser(const std::string& username, const std::string& password);
    bool canManageInventory() const override { return true; }
    bool canManageUsers() const override { return true; }
};

class RegularUser : public User {
public:
    RegularUser(const std::string& username, const std::string& password);
    bool canManageInventory() const override { return false; }
    bool canManageUsers() const override { return false; }
};

#endif // USER_H
