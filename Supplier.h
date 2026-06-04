#ifndef SUPPLIER_H
#define SUPPLIER_H

#include <string>
#include <iostream>

class Supplier {
private:
    std::string name;
    std::string contactPerson;
    std::string phone;
    std::string email;

public:
    Supplier();
    Supplier(const std::string& name, const std::string& contactPerson, const std::string& phone, const std::string& email);

    std::string getName() const { return name; }
    std::string getContactPerson() const { return contactPerson; }
    std::string getPhone() const { return phone; }
    std::string getEmail() const { return email; }

    void setName(const std::string& name) { this->name = name; }
    void setContactPerson(const std::string& contact) { this->contactPerson = contact; }
    void setPhone(const std::string& phone) { this->phone = phone; }
    void setEmail(const std::string& email) { this->email = email; }

    friend std::ostream& operator<<(std::ostream& os, const Supplier& supplier);
    friend std::istream& operator>>(std::istream& is, Supplier& supplier);
};

#endif