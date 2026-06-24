#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <vector>
#include <memory>
#include "Cargo.h"
#include "User.h"

class Warehouse {
private:
    std::vector<std::shared_ptr<Cargo>> inventory;
    std::vector<std::shared_ptr<User>> users;
    std::shared_ptr<User> currentUser;

public:
    Warehouse();

    // 帳密登入與管制
    bool login(const std::string& username, const std::string& password);
    void logout();
    std::shared_ptr<User> getCurrentUser() const { return currentUser; }
    bool isLoggedIn() const { return currentUser != nullptr; }

    // 貨物管理 API
    bool addCargo(std::shared_ptr<Cargo> cargo);
    bool removeCargo(const std::string& id);
    std::shared_ptr<Cargo> findCargo(const std::string& id) const;
    const std::vector<std::shared_ptr<Cargo>>& getInventory() const { return inventory; }
    
    // 清空倉庫 (用於讀檔前重置)
    void clearInventory() { inventory.clear(); }

    // 使用者管理 API
    bool addUser(std::shared_ptr<User> user);
    std::shared_ptr<User> findUser(const std::string& username) const;
    const std::vector<std::shared_ptr<User>>& getUsers() const { return users; }
    void clearUsers() { users.clear(); }

    // 統計計算 (展現 STL 演算法與多型運用)
    double calculateTotalVolume() const;
    double calculateTotalRevenue() const;
};

#endif // WAREHOUSE_H
