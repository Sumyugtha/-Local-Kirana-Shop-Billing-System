#ifndef GROCERY_H
#define GROCERY_H

#include "Item.h"

class Grocery : public Item {
public:
    Grocery();
    Grocery(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName);

    double getDiscountedPrice(int purchaseQty) const override;
    std::string getCategoryName() const override;
};

#endif