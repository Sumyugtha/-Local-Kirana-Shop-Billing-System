#include "../include/Snack.h"

Snack::Snack() : Item() {}

Snack::Snack(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName)
    : Item(id, name, price, quantity, threshold, supplierName) {}

double Snack::getDiscountedPrice(int purchaseQty) const {
    return price * 0.90;
}

std::string Snack::getCategoryName() const {
    return "Snack";
}