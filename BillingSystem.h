#ifndef BILLING_SYSTEM_H
#define BILLING_SYSTEM_H

#include "Item.h"
#include "InventoryManager.h"
#include <string>
#include <vector>
#include <iostream>

struct BillItem {
    int itemId;
    std::string name;
    std::string category;
    double price;
    double discountedPrice;
    int quantity;
};

class Bill {
private:
    int billId;
    std::string customerName;
    std::string customerPhone;
    std::string timestamp;
    std::vector<BillItem> items;
    double totalOriginalAmount;
    double totalDiscountedAmount;

public:
    Bill();
    Bill(int id, const std::string& name, const std::string& phone);

    void addBillItem(const Item& item, int qty);
    
    int getId() const { return billId; }
    std::string getCustomerName() const { return customerName; }
    std::string getCustomerPhone() const { return customerPhone; }
    std::string getTimestamp() const { return timestamp; }
    const std::vector<BillItem>& getItems() const { return items; }
    double getTotalOriginal() const { return totalOriginalAmount; }
    double getTotalDiscounted() const { return totalDiscountedAmount; }
    double getTotalSavings() const { return totalOriginalAmount - totalDiscountedAmount; }

    void setId(int id) { billId = id; }
    void setCustomerName(const std::string& name) { customerName = name; }
    void setCustomerPhone(const std::string& phone) { customerPhone = phone; }
    void setTimestamp(const std::string& ts) { timestamp = ts; }
    void setTotalOriginal(double amt) { totalOriginalAmount = amt; }
    void setTotalDiscounted(double amt) { totalDiscountedAmount = amt; }
    void addRawItem(const BillItem& item) { items.push_back(item); }

    friend std::ostream& operator<<(std::ostream& os, const Bill& bill);
};

class BillingSystem {
private:
    std::string salesLogFile;
    int nextBillId;

    std::string getCurrentDateTime() const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;

public:
    BillingSystem(const std::string& salesFile);

    Bill createNewBill(const std::string& custName, const std::string& custPhone);
    bool saveBillToFile(const Bill& bill);
    std::vector<Bill> loadAllBills() const;

    void generateDailySalesReport(const std::string& targetDate = "") const;
};

#endif