#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <memory>
#include "Warehouse.h"

class Database {
private:
    std::string usersFilePath;
    std::string cargoFilePath;

    // 輔助函式：分割字串
    static std::vector<std::string> split(const std::string& s, char delimiter);

public:
    Database(const std::string& usersPath = "data/users.txt", const std::string& cargoPath = "data/cargo.txt");

    // 載入與儲存使用者
    bool loadUsers(Warehouse& warehouse);
    bool saveUsers(const Warehouse& warehouse);

    // 載入與儲存貨物
    bool loadCargo(Warehouse& warehouse);
    bool saveCargo(const Warehouse& warehouse);
};

#endif // DATABASE_H
