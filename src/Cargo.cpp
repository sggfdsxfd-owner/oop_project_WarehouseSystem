#include "Cargo.h"
#include <iomanip>

Cargo::Cargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, const std::string& owner)
    : id(id), name(name), weight(weight), volume(volume), baseRate(baseRate), owner(owner) {}

double Cargo::calculateStorageFee() const {
    return volume * baseRate;
}

void Cargo::printDetails() const {
    std::cout << std::left << std::setw(8) << id
              << std::setw(15) << name
              << std::setw(10) << getType()
              << std::right << std::setw(10) << std::fixed << std::setprecision(2) << weight << " kg"
              << std::setw(10) << volume << " m³"
              << std::setw(12) << calculateStorageFee() << " 元 ("
              << "擁有者: " << owner << ")" << std::endl;
}
