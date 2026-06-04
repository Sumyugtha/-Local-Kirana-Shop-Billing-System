#ifndef SNACK_H
#define SNACK_H

#include "Item.h"

class Snack : public Item {
public:
    Snack();
    Snack(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName);

    double getDiscountedPrice(int purchaseQty) const override;
    std::string getCategoryName() const override;
};

#endif