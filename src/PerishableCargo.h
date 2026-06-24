#ifndef PERISHABLECARGO_H
#define PERISHABLECARGO_H

#include "Cargo.h"

class PerishableCargo : public Cargo {
private:
    std::string expiryDate;  // 格式: YYYY-MM-DD
    double requiredTemp;     // 溫控需求 (單位: °C)

public:
    PerishableCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                    const std::string& owner, const std::string& expiryDate, double requiredTemp);

    double calculateStorageFee() const override;
    void printDetails() const override;

    std::string getExpiryDate() const { return expiryDate; }
    double getRequiredTemp() const { return requiredTemp; }
    std::string getType() const override { return "Perishable"; }
};

#endif // PERISHABLECARGO_H
