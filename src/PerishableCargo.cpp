#include "PerishableCargo.h"
#include <iomanip>
#include <cmath>

PerishableCargo::PerishableCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                                 const std::string& owner, const std::string& expiryDate, double requiredTemp)
    : Cargo(id, name, weight, volume, baseRate, owner), expiryDate(expiryDate), requiredTemp(requiredTemp) {}

double PerishableCargo::calculateStorageFee() const {
    double baseFee = volume * baseRate;
    double coolingFee = 0.0;

    // 依據溫度加收冷鏈電費費率
    if (requiredTemp < 0.0) {
        coolingFee = volume * 300.0; // 冷凍級
    } else if (requiredTemp <= 8.0) {
        coolingFee = volume * 150.0; // 冷藏級
    }

    return baseFee + coolingFee;
}

void PerishableCargo::printDetails() const {
    Cargo::printDetails();
    std::cout << "  ↳ [易腐品] 保存期限: " << expiryDate 
              << " | 溫控需求: " << requiredTemp << " °C" 
              << " | (含冷鏈電力成本加成)" << std::endl;
}
