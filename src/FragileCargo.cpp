#include "FragileCargo.h"
#include <iomanip>

FragileCargo::FragileCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                           const std::string& owner, const std::string& category, const std::string& listingTime, const std::string& status,
                           const std::string& packagingType, int maxStackHeight)
    : Cargo(id, name, weight, volume, baseRate, owner, category, listingTime, status), packagingType(packagingType), maxStackHeight(maxStackHeight) {}

double FragileCargo::calculateStorageFee() const {
    double baseFee = volume * baseRate;
    double careFee = 0.0;

    // 堆疊限制越低，占用越多倉庫地板面積，需加收補償費
    if (maxStackHeight <= 2) {
        careFee += volume * 200.0;
    } else {
        careFee += volume * 50.0;
    }

    // 依包裝類別加收特殊處理費
    if (packagingType == "木箱" || packagingType == "Wooden Crate") {
        careFee += 100.0;
    } else if (packagingType == "氣泡紙" || packagingType == "Bubble Wrap") {
        careFee += 50.0;
    } else {
        careFee += 30.0;
    }

    return baseFee + careFee;
}

void FragileCargo::printDetails() const {
    Cargo::printDetails();
    std::cout << "  ↳ [易碎品] 包裝型態: " << packagingType 
              << " | 限堆疊層數: " << maxStackHeight << " 層" 
              << " | (含低堆疊空間占用補償費)" << std::endl;
}
