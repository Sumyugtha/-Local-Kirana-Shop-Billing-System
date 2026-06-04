#include "../include/Grocery.h"

Grocery::Grocery() : Item() {}

Grocery::Grocery(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName)
    : Item(id, name, price, quantity, threshold, supplierName) {}

double Grocery::getDiscountedPrice(int purchaseQty) const {
    return price * 0.95;
}

std::string Grocery::getCategoryName() const {
    return "Grocery";
}