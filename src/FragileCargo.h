#ifndef FRAGILECARGO_H
#define FRAGILECARGO_H

#include "Cargo.h"

class FragileCargo : public Cargo {
private:
    std::string packagingType; // 包裝方式 (例如: 木箱, 氣泡紙, 紙箱)
    int maxStackHeight;        // 最大堆疊層數 (防止重壓)

public:
    FragileCargo(const std::string& id, const std::string& name, double weight, double volume, double baseRate, 
                 const std::string& owner, const std::string& packagingType, int maxStackHeight);

    double calculateStorageFee() const override;
    void printDetails() const override;

    std::string getPackagingType() const { return packagingType; }
    int getMaxStackHeight() const { return maxStackHeight; }
    std::string getType() const override { return "Fragile"; }
};

#endif // FRAGILECARGO_H
