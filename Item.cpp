#include "../include/Item.h"
#include <iomanip>

Item::Item() : id(0), name(""), price(0.0), quantity(0), threshold(0), supplierName("") {}

Item::Item(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName)
    : id(id), name(name), price(price), quantity(quantity), threshold(threshold), supplierName(supplierName) {}

void Item::print(std::ostream& os) const {
    os << std::left 
       << std::setw(6) << id 
       << std::setw(20) << name 
       << std::setw(15) << getCategoryName()
       << std::right << std::setw(10) << std::fixed << std::setprecision(2) << price
       << std::setw(10) << quantity
       << std::setw(12) << threshold 
       << "   " << std::left << supplierName;
}

void Item::read(std::istream& is) {
    std::cout << "Enter Product Name: ";
    std::getline(is, name);
    if (name.empty()) {
        std::getline(is, name);
    }
    std::cout << "Enter Price (Rs.): ";
    is >> price;
    std::cout << "Enter Initial Stock Quantity: ";
    is >> quantity;
    std::cout << "Enter Low Stock Alert Threshold: ";
    is >> threshold;
    is.ignore();
    std::cout << "Enter Supplier Name: ";
    std::getline(is, supplierName);
}

std::ostream& operator<<(std::ostream& os, const Item& item) {
    item.print(os);
    return os;
}

std::istream& operator>>(std::istream& is, Item& item) {
    item.read(is);
    return is;
}