#include "DangerousCargo.h"
#include <iomanip>

DangerousCargo::DangerousCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                               const std::string& owner, int hazardLevel, const std::string& unNumber)
    : Cargo(id, name, weight, volume, baseRate, owner), hazardLevel(hazardLevel), unNumber(unNumber) {}

double DangerousCargo::calculateStorageFee() const {
    // 額外加收：危險等級加成費率 + 固定安全監控保險費 500 元
    return (volume * baseRate * (1.0 + 0.2 * hazardLevel)) + 500.0;
}

void DangerousCargo::printDetails() const {
    Cargo::printDetails();
    std::cout << "  ↳ [危險品] 危害等級: " << hazardLevel 
              << " | UN編號: " << unNumber 
              << " | (含安全監控與保險加成費)" << std::endl;
}
