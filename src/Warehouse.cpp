#include "Warehouse.h"
#include <algorithm>
#include <iostream>

Warehouse::Warehouse() : currentUser(nullptr) {}

bool Warehouse::login(const std::string& username, const std::string& password) {
    auto user = findUser(username);
    if (user && user->verifyPassword(password)) {
        currentUser = user;
        return true;
    }
    return false;
}

void Warehouse::logout() {
    currentUser = nullptr;
}

bool Warehouse::addCargo(std::shared_ptr<Cargo> cargo) {
    if (!cargo) return false;
    // 檢查 ID 是否重複
    if (findCargo(cargo->getId()) != nullptr) {
        return false;
    }
    inventory.push_back(cargo);
    return true;
}

bool Warehouse::removeCargo(const std::string& id) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&id](const std::shared_ptr<Cargo>& c) { return c->getId() == id; });
    
    if (it != inventory.end()) {
        auto cargo = *it;
        if (!cargo->isListed()) return false;
        cargo->setStatus("Unlisted");
        return true;
    }
    return false;
}

bool Warehouse::relistCargo(const std::string& id) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&id](const std::shared_ptr<Cargo>& c) { return c->getId() == id; });
    
    if (it != inventory.end()) {
        auto cargo = *it;
        if (cargo->isListed()) return false;
        cargo->setStatus("Active");
        return true;
    }
    return false;
}

std::shared_ptr<Cargo> Warehouse::findCargo(const std::string& id) const {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&id](const std::shared_ptr<Cargo>& c) { return c->getId() == id; });
    
    if (it != inventory.end()) {
        return *it;
    }
    return nullptr;
}

bool Warehouse::addUser(std::shared_ptr<User> user) {
    if (!user) return false;
    // 檢查帳號是否重複
    if (findUser(user->getUsername()) != nullptr) {
        return false;
    }
    users.push_back(user);
    return true;
}

std::shared_ptr<User> Warehouse::findUser(const std::string& username) const {
    auto it = std::find_if(users.begin(), users.end(),
        [&username](const std::shared_ptr<User>& u) { return u->getUsername() == username; });
    
    if (it != users.end()) {
        return *it;
    }
    return nullptr;
}

double Warehouse::calculateTotalVolume() const {
    double total = 0.0;
    for (const auto& cargo : inventory) {
        total += cargo->getVolume();
    }
    return total;
}

double Warehouse::calculateTotalRevenue() const {
    double total = 0.0;
    for (const auto& cargo : inventory) {
        // 多型呼叫各個衍生類別的 calculateStorageFee
        total += cargo->calculateStorageFee();
    }
    return total;
}
