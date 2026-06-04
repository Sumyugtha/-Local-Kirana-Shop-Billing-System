#include "../include/Supplier.h"
#include <iomanip>

Supplier::Supplier() : name(""), contactPerson(""), phone(""), email("") {}

Supplier::Supplier(const std::string& name, const std::string& contactPerson, const std::string& phone, const std::string& email)
    : name(name), contactPerson(contactPerson), phone(phone), email(email) {}

std::ostream& operator<<(std::ostream& os, const Supplier& supplier) {
    os << std::left << std::setw(20) << supplier.name 
       << std::setw(20) << supplier.contactPerson 
       << std::setw(15) << supplier.phone 
       << supplier.email;
    return os;
}

std::istream& operator>>(std::istream& is, Supplier& supplier) {
    std::cout << "Enter Supplier Name: ";
    std::getline(is, supplier.name);
    std::cout << "Enter Contact Person: ";
    std::getline(is, supplier.contactPerson);
    std::cout << "Enter Phone Number: ";
    std::getline(is, supplier.phone);
    std::cout << "Enter Email Address: ";
    std::getline(is, supplier.email);
    return is;
}