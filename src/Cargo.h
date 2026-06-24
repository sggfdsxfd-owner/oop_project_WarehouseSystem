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
    std::string category;    // 貨物分類
    std::string listingTime; // 上架時間
    std::string status;      // Active / Unlisted

public:
    Cargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, const std::string& owner,
          const std::string& category = "", const std::string& listingTime = "", const std::string& status = "Active");
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
    std::string getCategory() const { return category; }
    std::string getListingTime() const { return listingTime; }
    std::string getStatus() const { return status; }
    bool isListed() const { return status == "Active"; }
    virtual std::string getType() const { return "General"; }

    void setName(const std::string& name) { this->name = name; }
    void setWeight(double weight) { this->weight = weight; }
    void setVolume(double volume) { this->volume = volume; }
    void setBaseRate(double baseRate) { this->baseRate = baseRate; }
    void setCategory(const std::string& category) { this->category = category; }
    void setListingTime(const std::string& listingTime) { this->listingTime = listingTime; }
    void setStatus(const std::string& status) { this->status = status; }
};

#endif // CARGO_H
