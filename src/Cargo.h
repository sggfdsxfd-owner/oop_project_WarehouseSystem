#ifndef CARGO_H
#define CARGO_H

#include <string>
#include <iostream>

class Cargo {
protected:
    std::string id;
    std::string name;
    double weight;   // 單位：kg
    double volume;   // 單位：立方公尺 (m³)
    double baseRate; // 基本費率/月/立方公尺
    std::string owner; // 貨物擁有者的使用者名稱

public:
    Cargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, const std::string& owner);
    virtual ~Cargo() = default;

    // 計算倉儲費用 (多型基礎)
    virtual double calculateStorageFee() const;

    // 顯示詳細資訊
    virtual void printDetails() const;

    // Getters & Setters
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getWeight() const { return weight; }
    double getVolume() const { return volume; }
    double getBaseRate() const { return baseRate; }
    std::string getOwner() const { return owner; }
    virtual std::string getType() const { return "General"; }

    void setName(const std::string& name) { this->name = name; }
    void setWeight(double weight) { this->weight = weight; }
    void setVolume(double volume) { this->volume = volume; }
    void setBaseRate(double baseRate) { this->baseRate = baseRate; }
};

#endif // CARGO_H
