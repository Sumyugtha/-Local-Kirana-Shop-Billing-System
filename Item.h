#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

class Item {
protected:
    int id;
    std::string name;
    double price;
    int quantity;
    int threshold;
    std::string supplierName;

public:
    Item();
    Item(int id, const std::string& name, double price, int quantity, int threshold, const std::string& supplierName);
    virtual ~Item() = default;

    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    int getThreshold() const { return threshold; }
    std::string getSupplierName() const { return supplierName; }

    void setId(int id) { this->id = id; }
    void setName(const std::string& name) { this->name = name; }
    void setPrice(double price) { this->price = price; }
    void setQuantity(int qty) { this->quantity = qty; }
    void setThreshold(int thresh) { this->threshold = thresh; }
    void setSupplierName(const std::string& supplier) { this->supplierName = supplier; }

    void decreaseStock(int qty) { this->quantity -= qty; }
    void increaseStock(int qty) { this->quantity += qty; }
    bool isLowStock() const { return quantity <= threshold; }

    virtual double getDiscountedPrice(int purchaseQty) const = 0;
    virtual std::string getCategoryName() const = 0;

    virtual void print(std::ostream& os) const;
    virtual void read(std::istream& is);

    friend std::ostream& operator<<(std::ostream& os, const Item& item);
    friend std::istream& operator>>(std::istream& is, Item& item);
};

#endif