#define _CRT_SECURE_NO_WARNINGS
#include "../include/BillingSystem.h"
#include "../include/CLI_Helper.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <map>

Bill::Bill() : billId(0), customerName(""), customerPhone(""), timestamp(""), totalOriginalAmount(0.0), totalDiscountedAmount(0.0) {}

Bill::Bill(int id, const std::string& name, const std::string& phone)
    : billId(id), customerName(name), customerPhone(phone), totalOriginalAmount(0.0), totalDiscountedAmount(0.0) {}

void Bill::addBillItem(const Item& item, int qty) {
    BillItem bi;
    bi.itemId = item.getId();
    bi.name = item.getName();
    bi.category = item.getCategoryName();
    bi.price = item.getPrice();
    bi.discountedPrice = item.getDiscountedPrice(qty);
    bi.quantity = qty;

    items.push_back(bi);
    totalOriginalAmount += (bi.price * qty);
    totalDiscountedAmount += (bi.discountedPrice * qty);
}

std::ostream& operator<<(std::ostream& os, const Bill& bill) {
    bool isConsole = (&os == &std::cout);
    std::string b_cyan = isConsole ? CLI::BOLD_CYAN : "";
    std::string b_green = isConsole ? CLI::BOLD_GREEN : "";
    std::string b_white = isConsole ? CLI::BOLD_WHITE : "";
    std::string yellow = isConsole ? CLI::YELLOW : "";
    std::string reset = isConsole ? CLI::RESET : "";

    os << b_cyan << "======================================================================\n"
       << "||                       KIRANA BILLING RECEIPT                     ||\n"
       << "======================================================================\n" << reset;
    os << "Bill ID  : " << b_white << bill.billId << reset << "\n";
    os << "Date/Time: " << bill.timestamp << "\n";
    os << "Customer : " << b_white << (bill.customerName.empty() ? "Walk-in Customer" : bill.customerName)
       << " (" << (bill.customerPhone.empty() ? "N/A" : bill.customerPhone) << ")" << reset << "\n";

    os << b_cyan << "----------------------------------------------------------------------\n" << reset;
    os << std::left << std::setw(6) << "ID"
       << std::setw(22) << "Item Name"
       << std::setw(12) << "Category"
       << std::right << std::setw(6) << "Qty"
       << std::setw(10) << "Price"
       << std::setw(12) << "Total (Rs)" << "\n";
    os << b_cyan << "----------------------------------------------------------------------\n" << reset;

    for (const auto& item : bill.items) {
        double finalTotal = item.discountedPrice * item.quantity;

        os << std::left << std::setw(6) << item.itemId
           << std::setw(22) << (item.name.length() > 20 ? item.name.substr(0, 18) + ".." : item.name)
           << std::setw(12) << item.category
           << std::right << std::setw(6) << item.quantity
           << std::setw(10) << std::fixed << std::setprecision(2) << item.price
           << std::setw(12) << finalTotal << "\n";

        if (item.discountedPrice < item.price) {
            double saving = (item.price - item.discountedPrice) * item.quantity;
            os << std::left << std::setw(6) << ""
               << yellow << "  [Promo Discount applied: Save Rs. " << std::fixed << std::setprecision(2) << saving << "]" << reset << "\n";
        }
    }

    os << b_cyan << "----------------------------------------------------------------------\n" << reset;
    os << std::right << std::setw(46) << "Subtotal Amount: Rs. " << std::setw(10) << bill.totalOriginalAmount << "\n";
    os << yellow << std::right << std::setw(46) << "Total Discounts: Rs. " << std::setw(10) << bill.getTotalSavings() << reset << "\n";
    os << b_cyan << "----------------------------------------------------------------------\n" << reset;
    os << b_green << std::right << std::setw(46) << "GRAND TOTAL: Rs. " << std::setw(10) << bill.totalDiscountedAmount << reset << "\n";
    os << b_cyan << "======================================================================\n" << reset;
    os << "               Thank you for shopping! Visit again.                   \n";
    os << b_cyan << "======================================================================\n" << reset;

    return os;
}

BillingSystem::BillingSystem(const std::string& salesFile) : salesLogFile(salesFile), nextBillId(10001) {
    std::ifstream file(salesLogFile);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            if (line.substr(0, 5) == "BILL|") {
                std::vector<std::string> tokens = split(line, '|');
                if (tokens.size() > 1) {
                    int id = std::stoi(tokens[1]);
                    if (id >= nextBillId) {
                        nextBillId = id + 1;
                    }
                }
            }
        }
        file.close();
    }
}

std::string BillingSystem::getCurrentDateTime() const {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", local);
    return std::string(buf);
}

std::vector<std::string> BillingSystem::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

Bill BillingSystem::createNewBill(const std::string& custName, const std::string& custPhone) {
    Bill bill(nextBillId++, custName, custPhone);
    bill.setTimestamp(getCurrentDateTime());
    return bill;
}

bool BillingSystem::saveBillToFile(const Bill& bill) {
    std::ofstream file(salesLogFile, std::ios::app);
    if (!file.is_open()) {
        return false;
    }

    file << "BILL|" << bill.getId() << "|"
         << bill.getCustomerName() << "|"
         << bill.getCustomerPhone() << "|"
         << bill.getTimestamp() << "|"
         << bill.getTotalOriginal() << "|"
         << bill.getTotalDiscounted() << "\n";

    for (const auto& item : bill.getItems()) {
        file << "ITEM|" << item.itemId << "|"
             << item.name << "|"
             << item.category << "|"
             << item.price << "|"
             << item.discountedPrice << "|"
             << item.quantity << "\n";
    }
    file << "ENDBILL\n";
    file.close();
    return true;
}

std::vector<Bill> BillingSystem::loadAllBills() const {
    std::vector<Bill> bills;
    std::ifstream file(salesLogFile);
    if (!file.is_open()) {
        return bills;
    }

    std::string line;
    Bill currentBill;
    bool inBill = false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = split(line, '|');
        if (tokens[0] == "BILL") {
            int id = std::stoi(tokens[1]);
            std::string name = tokens[2];
            std::string phone = tokens[3];
            std::string ts = tokens[4];
            double orig = std::stod(tokens[5]);
            double disc = std::stod(tokens[6]);

            currentBill = Bill(id, name, phone);
            currentBill.setTimestamp(ts);
            currentBill.setTotalOriginal(orig);
            currentBill.setTotalDiscounted(disc);
            inBill = true;
        }
        else if (tokens[0] == "ITEM" && inBill) {
            BillItem bi;
            bi.itemId = std::stoi(tokens[1]);
            bi.name = tokens[2];
            bi.category = tokens[3];
            bi.price = std::stod(tokens[4]);
            bi.discountedPrice = std::stod(tokens[5]);
            bi.quantity = std::stoi(tokens[6]);
            currentBill.addRawItem(bi);
        }
        else if (tokens[0] == "ENDBILL" && inBill) {
            bills.push_back(currentBill);
            inBill = false;
        }
    }
    file.close();
    return bills;
}

void BillingSystem::generateDailySalesReport(const std::string& targetDate) const {
    std::string filterDate = targetDate;
    if (filterDate.empty()) {
        std::string nowTs = getCurrentDateTime();
        filterDate = nowTs.substr(0, 10);
    }

    std::vector<Bill> allBills = loadAllBills();

    int billCount = 0;
    int itemsSold = 0;
    double grossSales = 0.0;
    double netRevenue = 0.0;

    std::map<std::string, int> catQuantities;
    std::map<std::string, double> catRevenue;

    for (const auto& bill : allBills) {
        if (bill.getTimestamp().substr(0, 10) == filterDate) {
            billCount++;
            grossSales += bill.getTotalOriginal();
            netRevenue += bill.getTotalDiscounted();

            for (const auto& item : bill.getItems()) {
                itemsSold += item.quantity;
                catQuantities[item.category] += item.quantity;
                catRevenue[item.category] += (item.discountedPrice * item.quantity);
            }
        }
    }

    CLI::printHeader("DAILY SALES SUMMARY: " + filterDate, CLI::BOLD_GREEN);

    if (billCount == 0) {
        std::cout << CLI::YELLOW << "\n   No sales recorded for this date: " << filterDate << CLI::RESET << "\n\n";
        return;
    }

    std::cout << "  Total Bills Generated   : " << CLI::BOLD_WHITE << billCount << CLI::RESET << "\n";
    std::cout << "  Total Items Sold        : " << CLI::BOLD_WHITE << itemsSold << CLI::RESET << "\n";
    std::cout << "  Gross Sales (Original)  : Rs. " << CLI::BOLD_WHITE << std::fixed << std::setprecision(2) << grossSales << CLI::RESET << "\n";
    std::cout << "  Net Revenue (Collected) : Rs. " << CLI::BOLD_GREEN << std::fixed << std::setprecision(2) << netRevenue << CLI::RESET << "\n";

    double savings = grossSales - netRevenue;
    double savingPercentage = grossSales > 0 ? (savings / grossSales) * 100.0 : 0.0;
    std::cout << "  Total Discounts Given   : Rs. " << CLI::YELLOW << savings
              << " (" << std::fixed << std::setprecision(1) << savingPercentage << "% customer savings)" << CLI::RESET << "\n";

    CLI::printLine('-', 70, CLI::CYAN);
    std::cout << CLI::BOLD_WHITE << "  Category-wise Sales breakdown:" << CLI::RESET << "\n";
    CLI::printLine('-', 70, CLI::CYAN);

    std::cout << std::left << std::setw(20) << "  Category"
              << std::right << std::setw(15) << "Items Sold"
              << std::setw(20) << "Net Revenue (Rs.)" << "\n";
    CLI::printLine('-', 70, CLI::CYAN);

    std::vector<std::string> standardCategories = {"Grocery", "Snack", "Beverage"};
    for (const auto& cat : standardCategories) {
        int qty = catQuantities[cat];
        double rev = catRevenue[cat];
        std::cout << "  " << std::left << std::setw(18) << cat
                  << std::right << std::setw(15) << qty
                  << std::setw(20) << std::fixed << std::setprecision(2) << rev << "\n";
    }

    for (const auto& entry : catRevenue) {
        if (entry.first != "Grocery" && entry.first != "Snack" && entry.first != "Beverage") {
            std::cout << "  " << std::left << std::setw(18) << entry.first
                      << std::right << std::setw(15) << catQuantities[entry.first]
                      << std::setw(20) << std::fixed << std::setprecision(2) << entry.second << "\n";
        }
    }

    CLI::printLine('=', 70, CLI::BOLD_GREEN);
    std::cout << "\n";
}
