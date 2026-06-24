#include "Database.h"
#include "DangerousCargo.h"
#include "PerishableCargo.h"
#include "FragileCargo.h"
#include <fstream>
#include <sstream>
#include <iostream>

Database::Database(const std::string& usersPath, const std::string& cargoPath)
    : usersFilePath(usersPath), cargoFilePath(cargoPath) {}

std::vector<std::string> Database::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool Database::loadUsers(Warehouse& warehouse) {
    warehouse.clearUsers();
    std::ifstream file(usersFilePath);

    // 如果檔案不存在，則寫入預設帳密並載入
    if (!file.is_open()) {
        std::ofstream outFile(usersFilePath);
        if (outFile.is_open()) {
            outFile << "Admin,admin,admin123" << std::endl;
            outFile << "Regular,user,user123" << std::endl;
            outFile.close();
        }
        file.open(usersFilePath);
        if (!file.is_open()) return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() < 3) continue;

        std::string role = tokens[0];
        std::string username = tokens[1];
        std::string password = tokens[2];

        if (role == "Admin") {
            warehouse.addUser(std::make_shared<AdminUser>(username, password));
        } else {
            warehouse.addUser(std::make_shared<RegularUser>(username, password));
        }
    }
    file.close();
    return true;
}

bool Database::saveUsers(const Warehouse& warehouse) {
    std::ofstream file(usersFilePath);
    if (!file.is_open()) return false;

    for (const auto& user : warehouse.getUsers()) {
        file << user->getRole() << ","
             << user->getUsername() << ","
             << user->getPassword() << std::endl;
    }
    file.close();
    return true;
}

bool Database::loadCargo(Warehouse& warehouse) {
    warehouse.clearInventory();
    std::ifstream file(cargoFilePath);
    if (!file.is_open()) {
        // 檔案不存在代表是空倉庫，回傳 true
        return true;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() < 7) continue;

        std::string type = tokens[0];
        std::string id = tokens[1];
        std::string name = tokens[2];
        double weight = std::stod(tokens[3]);
        double volume = std::stod(tokens[4]);
        double baseRate = std::stod(tokens[5]);
        std::string owner = tokens[6];
        std::string category = tokens.size() > 7 ? tokens[7] : "";
        std::string listingTime = tokens.size() > 8 ? tokens[8] : "";
        std::string status = tokens.size() > 9 ? tokens[9] : "Active";

        if (type == "General") {
            warehouse.addCargo(std::make_shared<Cargo>(id, name, weight, volume, baseRate, owner, category, listingTime, status));
        } else if (type == "Dangerous" && tokens.size() >= 11) {
            int hazardLevel = std::stoi(tokens[10]);
            std::string unNumber = tokens[11];
            warehouse.addCargo(std::make_shared<DangerousCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, status, hazardLevel, unNumber));
        } else if (type == "Perishable" && tokens.size() >= 11) {
            std::string expiryDate = tokens[10];
            double requiredTemp = std::stod(tokens[11]);
            warehouse.addCargo(std::make_shared<PerishableCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, status, expiryDate, requiredTemp));
        } else if (type == "Fragile" && tokens.size() >= 11) {
            std::string packagingType = tokens[10];
            int maxStackHeight = std::stoi(tokens[11]);
            warehouse.addCargo(std::make_shared<FragileCargo>(id, name, weight, volume, baseRate, owner, category, listingTime, status, packagingType, maxStackHeight));
        }
    }
    file.close();
    return true;
}

bool Database::saveCargo(const Warehouse& warehouse) {
    std::ofstream file(cargoFilePath);
    if (!file.is_open()) return false;

    for (const auto& cargo : warehouse.getInventory()) {
        file << cargo->getType() << ","
             << cargo->getId() << ","
             << cargo->getName() << ","
             << cargo->getWeight() << ","
             << cargo->getVolume() << ","
             << cargo->getBaseRate() << ","
             << cargo->getOwner() << ","
             << cargo->getCategory() << ","
             << cargo->getListingTime() << ","
             << cargo->getStatus();

        if (cargo->getType() == "Dangerous") {
            auto dc = std::dynamic_pointer_cast<DangerousCargo>(cargo);
            if (dc) {
                file << "," << dc->getHazardLevel() << "," << dc->getUnNumber();
            }
        } else if (cargo->getType() == "Perishable") {
            auto pc = std::dynamic_pointer_cast<PerishableCargo>(cargo);
            if (pc) {
                file << "," << pc->getExpiryDate() << "," << pc->getRequiredTemp();
            }
        } else if (cargo->getType() == "Fragile") {
            auto fc = std::dynamic_pointer_cast<FragileCargo>(cargo);
            if (fc) {
                file << "," << fc->getPackagingType() << "," << fc->getMaxStackHeight();
            }
        }
        file << std::endl;
    }
    file.close();
    return true;
}
