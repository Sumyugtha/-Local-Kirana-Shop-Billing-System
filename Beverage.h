#ifndef BEVERAGE_H
#define BEVERAGE_H

#include "Item.h"

class Beverage : public Item {
public:
    Beverage();
    Beverage(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName);

    double getDiscountedPrice(int purchaseQty) const override;
    std::string getCategoryName() const override;
};

#endif