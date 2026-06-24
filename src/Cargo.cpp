#include "Cargo.h"
#include <iomanip>

Cargo::Cargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, const std::string& owner,
               const std::string& category, const std::string& listingTime, const std::string& status)
    : id(id), name(name), weight(weight), volume(volume), baseRate(baseRate), owner(owner), category(category), listingTime(listingTime), status(status) {}

double Cargo::calculateStorageFee() const {
    return volume * baseRate;
}

void Cargo::printDetails() const {
    // 首行：主要欄位（ID / 名稱 / 類別 / 重量 / 容積 / 費用）
    std::cout << std::left << std::setw(8) << id
              << std::setw(15) << name
              << std::setw(10) << getType()
              << std::right << std::setw(10) << std::fixed << std::setprecision(2) << weight << " kg"
              << std::setw(10) << volume << " m³"
              << std::setw(12) << calculateStorageFee() << " 元" << std::endl;

    // 第二行：補充資訊（擁有者 / 分類 / 狀態 / 上架時間）
    std::cout << "       "
              << "擁有者: " << owner << " | "
              << "分類: " << (category.empty() ? "未設定" : category) << " | "
              << "狀態: " << status << std::endl;
    if (!listingTime.empty()) {
        std::cout << "       " << "上架時間: " << listingTime << std::endl;
    }
}
