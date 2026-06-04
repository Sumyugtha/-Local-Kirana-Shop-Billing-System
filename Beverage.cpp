#include "../include/Beverage.h"

Beverage::Beverage() : Item() {}

Beverage::Beverage(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName)
    : Item(id, name, price, quantity, threshold, supplierName) {}

double Beverage::getDiscountedPrice(int purchaseQty) const {
    if (purchaseQty >= 5) {
        return price * 0.92;
    } else {
        return price * 0.98;
    }
}

std::string Beverage::getCategoryName() const {
    return "Beverage";
}