#ifndef DANGEROUSCARGO_H
#define DANGEROUSCARGO_H

#include "Cargo.h"

class DangerousCargo : public Cargo {
private:
    int hazardLevel;        // 危險等級 (1-9)
    std::string unNumber;   // UN 聯合國危險貨物編號

public:
    DangerousCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                   const std::string& owner, int hazardLevel, const std::string& unNumber);

    double calculateStorageFee() const override;
    void printDetails() const override;
    
    int getHazardLevel() const { return hazardLevel; }
    std::string getUnNumber() const { return unNumber; }
    std::string getType() const override { return "Dangerous"; }
};

#endif // DANGEROUSCARGO_H
