#include "../include/CLI_Helper.h"
#include "../include/InventoryManager.h"
#include "../include/BillingSystem.h"
#include "../include/Grocery.h"
#include "../include/Snack.h"
#include "../include/Beverage.h"
#include <iostream>
#include <memory>
#include <iomanip>
#include <sstream>

const std::string INVENTORY_FILE = "data/inventory.txt";
const std::string SUPPLIERS_FILE = "data/suppliers.txt";
const std::string SALES_FILE     = "data/sales.txt";

void runBilling(InventoryManager& invMgr, BillingSystem& billSys) {
    CLI::clearScreen();
    CLI::printHeader("BILLING CHECKOUT MODULE", CLI::BOLD_CYAN);

    std::string name = CLI::readString("Enter Customer Name (Press Enter to skip): ", true);
    std::string phone = CLI::readString("Enter Customer Phone (Press Enter to skip): ", true);

    Bill bill = billSys.createNewBill(name, phone);

    struct CartItem {
        Item* item;
        int purchaseQty;
    };
    std::vector<CartItem> cart;

    while (true) {
        CLI::printLine('-', 70, CLI::CYAN);
        std::cout << "  Current Cart Size: " << CLI::BOLD_WHITE << cart.size() << CLI::RESET << " items\n";
        double runningTotal = 0.0;
        for (const auto& cartItem : cart) {
            runningTotal += (cartItem.item->getDiscountedPrice(cartItem.purchaseQty) * cartItem.purchaseQty);
        }
        std::cout << "  Running Total: " << CLI::BOLD_GREEN << "Rs. " << std::fixed << std::setprecision(2) << runningTotal << CLI::RESET << "\n";
        CLI::printLine('-', 70, CLI::CYAN);

        std::cout << "  Options:\n";
        std::cout << "  1. Add Product to Cart\n";
        std::cout << "  2. View Cart / Remove Item\n";
        std::cout << "  3. Checkout & Generate Bill\n";
        std::cout << "  4. Cancel Transaction\n";

        int opt = CLI::readInt("  Select option: ", 1, 4);

        if (opt == 1) {
            std::cout << "\n  Search for Product:\n";
            std::cout << "  1. By Product ID\n";
            std::cout << "  2. By Product Name\n";
            int searchOpt = CLI::readInt("  Select option: ", 1, 2);

            Item* product = nullptr;
            if (searchOpt == 1) {
                int id = CLI::readInt("  Enter Product ID: ");
                product = invMgr.findItemById(id);
            } else {
                std::string pName = CLI::readString("  Enter Product Name: ");
                product = invMgr.findItemByName(pName);
            }

            if (!product) {
                std::cout << CLI::RED << "  Product not found in inventory!" << CLI::RESET << "\n";
                continue;
            }

            int qtyInCart = 0;
            int cartIdx = -1;
            for (size_t i = 0; i < cart.size(); ++i) {
                if (cart[i].item->getId() == product->getId()) {
                    qtyInCart = cart[i].purchaseQty;
                    cartIdx = i;
                    break;
                }
            }

            int availableStock = product->getQuantity() - qtyInCart;
            std::cout << "\n  Product Found: " << CLI::BOLD_WHITE << product->getName() << CLI::RESET << "\n";
            std::cout << "  Category     : " << product->getCategoryName() << "\n";
            std::cout << "  Unit Price   : Rs. " << product->getPrice() << "\n";
            std::cout << "  Available Stock: " << (availableStock <= product->getThreshold() ? CLI::BOLD_RED : CLI::GREEN)
                      << availableStock << CLI::RESET << "\n\n";

            if (availableStock <= 0) {
                std::cout << CLI::RED << "  Cannot add product. Stock is depleted!" << CLI::RESET << "\n";
                continue;
            }

            int qtyToBuy = CLI::readInt("  Enter Quantity to purchase: ", 1, availableStock);

            if (cartIdx != -1) {
                cart[cartIdx].purchaseQty += qtyToBuy;
            } else {
                cart.push_back({product, qtyToBuy});
            }
            std::cout << CLI::BOLD_GREEN << "  Product added to cart!" << CLI::RESET << "\n";
        }
        else if (opt == 2) {
            if (cart.empty()) {
                std::cout << CLI::YELLOW << "  Your cart is empty!" << CLI::RESET << "\n";
                continue;
            }
            CLI::printLine('-', 70, CLI::CYAN);
            std::cout << std::left << std::setw(5) << "#"
                      << std::setw(6) << "ID"
                      << std::setw(25) << "Item Name"
                      << std::right << std::setw(8) << "Qty"
                      << std::setw(12) << "Disc Price"
                      << std::setw(12) << "Total" << "\n";
            CLI::printLine('-', 70, CLI::CYAN);
            for (size_t i = 0; i < cart.size(); ++i) {
                double dp = cart[i].item->getDiscountedPrice(cart[i].purchaseQty);
                std::cout << std::left << std::setw(5) << (i + 1)
                          << std::setw(6) << cart[i].item->getId()
                          << std::setw(25) << cart[i].item->getName()
                          << std::right << std::setw(8) << cart[i].purchaseQty
                          << std::setw(12) << std::fixed << std::setprecision(2) << dp
                          << std::setw(12) << (dp * cart[i].purchaseQty) << "\n";
            }
            CLI::printLine('-', 70, CLI::CYAN);
            std::cout << "  Enter item index (1-" << cart.size() << ") to REMOVE, or enter 0 to go back: ";
            int removeIdx = CLI::readInt("", 0, cart.size());
            if (removeIdx > 0) {
                cart.erase(cart.begin() + (removeIdx - 1));
                std::cout << CLI::GREEN << "  Item removed from cart." << CLI::RESET << "\n";
            }
        }
        else if (opt == 3) {
            if (cart.empty()) {
                std::cout << CLI::RED << "  Cannot checkout with empty cart!" << CLI::RESET << "\n";
                continue;
            }

            std::vector<std::string> lowStockWarnings;
            for (const auto& cartItem : cart) {
                bill.addBillItem(*(cartItem.item), cartItem.purchaseQty);
                cartItem.item->decreaseStock(cartItem.purchaseQty);

                if (cartItem.item->isLowStock()) {
                    std::stringstream ss;
                    ss << cartItem.item->getName() << " (ID: " << cartItem.item->getId()
                       << ") - Current Stock: " << cartItem.item->getQuantity()
                       << " (Threshold: " << cartItem.item->getThreshold() << ")"
                       << "\n     Supplier: " << cartItem.item->getSupplierName();
                    lowStockWarnings.push_back(ss.str());
                }
            }

            billSys.saveBillToFile(bill);
            invMgr.saveInventory(INVENTORY_FILE);

            CLI::clearScreen();
            std::cout << bill << "\n";

            if (!lowStockWarnings.empty()) {
                std::cout << CLI::BOLD_RED << "!!! LOW STOCK ALERTS !!!" << CLI::RESET << "\n";
                for (const auto& warn : lowStockWarnings) {
                    std::cout << CLI::RED << " * " << warn << CLI::RESET << "\n";
                }
                std::cout << "\n";
            }

            CLI::pressAnyKey();
            break;
        }
        else if (opt == 4) {
            std::cout << CLI::YELLOW << "  Transaction cancelled." << CLI::RESET << "\n";
            CLI::pressAnyKey();
            break;
        }
    }
}

void runInventoryMenu(InventoryManager& invMgr) {
    while (true) {
        CLI::clearScreen();
        CLI::printHeader("INVENTORY MANAGEMENT", CLI::BOLD_CYAN);
        std::cout << "  1. View All Products\n";
        std::cout << "  2. Add New Product\n";
        std::cout << "  3. Update Stock Quantity\n";
        std::cout << "  4. Delete Product\n";
        std::cout << "  5. View Low Stock Alerts\n";
        std::cout << "  6. Back to Main Menu\n";
        CLI::printLine('=', 70, CLI::CYAN);

        int opt = CLI::readInt("  Select Option: ", 1, 6);

        if (opt == 1) {
            CLI::clearScreen();
            CLI::printHeader("CURRENT PRODUCTS INVENTORY", CLI::BOLD_CYAN);
            std::cout << std::left
                      << std::setw(6) << "ID"
                      << std::setw(20) << "Product Name"
                      << std::setw(15) << "Category"
                      << std::right << std::setw(10) << "Price (Rs)"
                      << std::setw(10) << "Stock"
                      << std::setw(12) << "Threshold"
                      << "   Supplier Name\n";
            CLI::printLine('-', 85, CLI::CYAN);

            for (const auto& item : invMgr.getItems()) {
                if (item->isLowStock()) {
                    std::cout << CLI::BOLD_RED;
                }
                std::cout << *item;
                if (item->isLowStock()) {
                    std::cout << " [LOW STOCK]" << CLI::RESET;
                }
                std::cout << "\n";
            }
            CLI::printLine('=', 85, CLI::CYAN);
            CLI::pressAnyKey();
        }
        else if (opt == 2) {
            CLI::clearScreen();
            CLI::printHeader("ADD NEW PRODUCT", CLI::BOLD_CYAN);

            std::cout << "Select Product Category Type:\n";
            std::cout << "  1. Grocery (5% Flat Discount)\n";
            std::cout << "  2. Snack   (10% Flat Discount)\n";
            std::cout << "  3. Beverage (8% Bulk / 2% Flat Discount)\n";
            int typeOpt = CLI::readInt("  Select Type: ", 1, 3);

            std::unique_ptr<Item> item = nullptr;
            if (typeOpt == 1) {
                item = std::make_unique<Grocery>();
            } else if (typeOpt == 2) {
                item = std::make_unique<Snack>();
            } else if (typeOpt == 3) {
                item = std::make_unique<Beverage>();
            }

            if (item) {
                std::cin >> *item;

                const Supplier* sup = invMgr.findSupplierByName(item->getSupplierName());
                if (!sup && !item->getSupplierName().empty()) {
                    std::cout << CLI::YELLOW << "\n  Warning: Supplier '" << item->getSupplierName()
                              << "' is not in the system yet." << CLI::RESET << "\n";
                    std::cout << "  Would you like to register this supplier now? (1: Yes, 0: No): ";
                    int regSup = CLI::readInt("", 0, 1);
                    if (regSup == 1) {
                        std::string contact = CLI::readString("  Enter Contact Person Name: ");
                        std::string phone = CLI::readString("  Enter Phone: ");
                        std::string email = CLI::readString("  Enter Email: ");
                        invMgr.addSupplier(Supplier(item->getSupplierName(), contact, phone, email));
                        invMgr.saveSuppliers(SUPPLIERS_FILE);
                        std::cout << CLI::GREEN << "  Supplier successfully registered!\n" << CLI::RESET;
                    }
                }

                invMgr.addItem(std::move(item));
                invMgr.saveInventory(INVENTORY_FILE);
                std::cout << CLI::BOLD_GREEN << "\n  Product successfully added to inventory!" << CLI::RESET << "\n";
            }
            CLI::pressAnyKey();
        }
        else if (opt == 3) {
            CLI::clearScreen();
            CLI::printHeader("UPDATE PRODUCT STOCK", CLI::BOLD_CYAN);
            int id = CLI::readInt("  Enter Product ID: ");
            Item* item = invMgr.findItemById(id);

            if (!item) {
                std::cout << CLI::RED << "  Product not found!" << CLI::RESET << "\n";
            } else {
                std::cout << "\n  Product found: " << CLI::BOLD_WHITE << item->getName() << CLI::RESET << "\n";
                std::cout << "  Current Stock: " << item->getQuantity() << "\n\n";

                std::cout << "  Stock Action:\n";
                std::cout << "  1. Add Stock (Restock)\n";
                std::cout << "  2. Deduct Stock (Correction)\n";
                int action = CLI::readInt("  Select: ", 1, 2);

                int qty = CLI::readInt("  Enter Quantity: ", 1);
                if (action == 1) {
                    item->increaseStock(qty);
                    std::cout << CLI::GREEN << "  Stock updated successfully. New Stock: " << item->getQuantity() << CLI::RESET << "\n";
                } else {
                    if (qty > item->getQuantity()) {
                        std::cout << CLI::RED << "  Error: Cannot deduct more stock than available!" << CLI::RESET << "\n";
                    } else {
                        item->decreaseStock(qty);
                        std::cout << CLI::GREEN << "  Stock updated successfully. New Stock: " << item->getQuantity() << CLI::RESET << "\n";
                    }
                }
                invMgr.saveInventory(INVENTORY_FILE);
            }
            CLI::pressAnyKey();
        }
        else if (opt == 4) {
            CLI::clearScreen();
            CLI::printHeader("DELETE PRODUCT", CLI::BOLD_CYAN);
            int id = CLI::readInt("  Enter Product ID to delete: ");
            Item* item = invMgr.findItemById(id);

            if (!item) {
                std::cout << CLI::RED << "  Product not found!" << CLI::RESET << "\n";
            } else {
                std::cout << "\n  Are you sure you want to delete: " << CLI::BOLD_RED << item->getName()
                          << CLI::RESET << "? (1: Yes, 0: No): ";
                int confirm = CLI::readInt("", 0, 1);
                if (confirm == 1) {
                    if (invMgr.removeItem(id)) {
                        invMgr.saveInventory(INVENTORY_FILE);
                        std::cout << CLI::GREEN << "  Product successfully deleted from inventory." << CLI::RESET << "\n";
                    } else {
                        std::cout << CLI::RED << "  Error removing product." << CLI::RESET << "\n";
                    }
                } else {
                    std::cout << CLI::YELLOW << "  Deletion cancelled." << CLI::RESET << "\n";
                }
            }
            CLI::pressAnyKey();
        }
        else if (opt == 5) {
            CLI::clearScreen();
            CLI::printHeader("LOW STOCK ALERTS & REORDER LIST", CLI::BOLD_RED);
            std::vector<Item*> lowStock = invMgr.getLowStockItems();

            if (lowStock.empty()) {
                std::cout << CLI::GREEN << "\n  All products are sufficiently stocked. No alerts!" << CLI::RESET << "\n\n";
            } else {
                std::cout << std::left
                          << std::setw(6) << "ID"
                          << std::setw(20) << "Product Name"
                          << std::right << std::setw(10) << "Stock"
                          << std::setw(12) << "Threshold"
                          << "   " << std::left << std::setw(20) << "Supplier Name"
                          << "Contact Info\n";
                CLI::printLine('-', 90, CLI::RED);

                for (const auto& item : lowStock) {
                    std::cout << std::left
                              << std::setw(6) << item->getId()
                              << std::setw(20) << item->getName()
                              << std::right << std::setw(10) << item->getQuantity()
                              << std::setw(12) << item->getThreshold()
                              << "   " << std::left << std::setw(20) << item->getSupplierName();

                    const Supplier* sup = invMgr.findSupplierByName(item->getSupplierName());
                    if (sup) {
                        std::cout << sup->getContactPerson() << " (" << sup->getPhone() << ")";
                    } else {
                        std::cout << CLI::YELLOW << "Supplier not registered!" << CLI::RESET;
                    }
                    std::cout << "\n";
                }
                CLI::printLine('=', 90, CLI::RED);
            }
            CLI::pressAnyKey();
        }
        else {
            break;
        }
    }
}

void runSupplierMenu(InventoryManager& invMgr) {
    while (true) {
        CLI::clearScreen();
        CLI::printHeader("SUPPLIER TRACKING MODULE", CLI::BOLD_CYAN);
        std::cout << "  1. View Registered Suppliers\n";
        std::cout << "  2. Register New Supplier\n";
        std::cout << "  3. View Supplier Products Portfolio\n";
        std::cout << "  4. Back to Main Menu\n";
        CLI::printLine('=', 70, CLI::CYAN);

        int opt = CLI::readInt("  Select Option: ", 1, 4);

        if (opt == 1) {
            CLI::clearScreen();
            CLI::printHeader("REGISTERED SUPPLIERS", CLI::BOLD_CYAN);
            std::cout << std::left
                      << std::setw(20) << "Supplier Name"
                      << std::setw(20) << "Contact Person"
                      << std::setw(15) << "Phone Number"
                      << "Email Address\n";
            CLI::printLine('-', 75, CLI::CYAN);

            for (const auto& sup : invMgr.getSuppliers()) {
                std::cout << sup << "\n";
            }
            CLI::printLine('=', 75, CLI::CYAN);
            CLI::pressAnyKey();
        }
        else if (opt == 2) {
            CLI::clearScreen();
            CLI::printHeader("REGISTER NEW SUPPLIER", CLI::BOLD_CYAN);
            Supplier sup;
            std::cin >> sup;
            invMgr.addSupplier(sup);
            invMgr.saveSuppliers(SUPPLIERS_FILE);
            std::cout << CLI::BOLD_GREEN << "\n  Supplier registered successfully!" << CLI::RESET << "\n";
            CLI::pressAnyKey();
        }
        else if (opt == 3) {
            CLI::clearScreen();
            CLI::printHeader("SUPPLIER PRODUCT PORTFOLIOS", CLI::BOLD_CYAN);
            std::string sName = CLI::readString("  Enter Supplier Name: ");

            const Supplier* sup = invMgr.findSupplierByName(sName);
            if (!sup) {
                std::cout << CLI::RED << "  Supplier not found!" << CLI::RESET << "\n";
            } else {
                CLI::clearScreen();
                CLI::printHeader("PORTFOLIO: " + sup->getName(), CLI::BOLD_CYAN);
                std::cout << "  Contact: " << sup->getContactPerson() << " | Phone: " << sup->getPhone() << "\n";
                CLI::printLine('-', 70, CLI::CYAN);

                int itemCount = 0;
                std::cout << std::left
                          << std::setw(6) << "ID"
                          << std::setw(25) << "Product Name"
                          << std::setw(15) << "Category"
                          << std::right << std::setw(12) << "Stock"
                          << std::setw(12) << "Price (Rs)\n";
                CLI::printLine('-', 70, CLI::CYAN);

                for (const auto& item : invMgr.getItems()) {
                    if (item->getSupplierName() == sup->getName()) {
                        itemCount++;
                        if (item->isLowStock()) {
                            std::cout << CLI::BOLD_RED;
                        }
                        std::cout << std::left
                                  << std::setw(6) << item->getId()
                                  << std::setw(25) << item->getName()
                                  << std::setw(15) << item->getCategoryName()
                                  << std::right << std::setw(12) << item->getQuantity()
                                  << std::setw(12) << std::fixed << std::setprecision(2) << item->getPrice();
                        if (item->isLowStock()) {
                            std::cout << " [LOW]" << CLI::RESET;
                        }
                        std::cout << "\n";
                    }
                }
                CLI::printLine('=', 70, CLI::CYAN);
                std::cout << "  Total products supplied: " << CLI::BOLD_WHITE << itemCount << CLI::RESET << "\n";
            }
            CLI::pressAnyKey();
        }
        else {
            break;
        }
    }
}

void runReportsMenu(const BillingSystem& billSys) {
    while (true) {
        CLI::clearScreen();
        CLI::printHeader("SALES & REVENUE ANALYTICS", CLI::BOLD_CYAN);
        std::cout << "  1. View Today's Sales Report\n";
        std::cout << "  2. View Sales Report by Specific Date\n";
        std::cout << "  3. Back to Main Menu\n";
        CLI::printLine('=', 70, CLI::CYAN);

        int opt = CLI::readInt("  Select Option: ", 1, 3);

        if (opt == 1) {
            CLI::clearScreen();
            billSys.generateDailySalesReport("");
            CLI::pressAnyKey();
        }
        else if (opt == 2) {
            CLI::clearScreen();
            std::cout << "  Enter date (Format: YYYY-MM-DD): ";
            std::string sDate = CLI::readString("");
            if (sDate.length() != 10 || sDate[4] != '-' || sDate[7] != '-') {
                std::cout << CLI::RED << "  Invalid date format. Expected YYYY-MM-DD." << CLI::RESET << "\n";
            } else {
                CLI::clearScreen();
                billSys.generateDailySalesReport(sDate);
            }
            CLI::pressAnyKey();
        }
        else {
            break;
        }
    }
}

int main() {
#ifdef _WIN32
    std::system("if not exist data mkdir data");
#else
    std::system("mkdir -p data");
#endif

    InventoryManager invMgr;
    invMgr.loadInventory(INVENTORY_FILE);
    invMgr.loadSuppliers(SUPPLIERS_FILE);

    BillingSystem billSys(SALES_FILE);

    while (true) {
        CLI::clearScreen();
        CLI::printHeader("LOCAL KIRANA SHOP BILLING SYSTEM", CLI::BOLD_GREEN);

        std::vector<Item*> lowStock = invMgr.getLowStockItems();
        if (!lowStock.empty()) {
            std::cout << "   " << CLI::BOLD_RED << "[!] ALERT: " << lowStock.size()
                      << " item(s) are low in stock. Please check inventory." << CLI::RESET << "\n\n";
        } else {
            std::cout << "   " << CLI::GREEN << "[+] Inventory health: Adequate stock." << CLI::RESET << "\n\n";
        }

        std::cout << "  1.  Generate Customer Bill (Checkout)\n";
        std::cout << "  2.  Product Inventory Management\n";
        std::cout << "  3.  Supplier Tracking & Portfolios\n";
        std::cout << "  4.  Sales Reports & Analytics\n";
        std::cout << "  5.  Exit System\n";

        CLI::printLine('=', 70, CLI::BOLD_GREEN);
        int opt = CLI::readInt("  Select module [1-5]: ", 1, 5);

        if (opt == 1) {
            runBilling(invMgr, billSys);
        } else if (opt == 2) {
            runInventoryMenu(invMgr);
        } else if (opt == 3) {
            runSupplierMenu(invMgr);
        } else if (opt == 4) {
            runReportsMenu(billSys);
        } else {
            invMgr.saveInventory(INVENTORY_FILE);
            invMgr.saveSuppliers(SUPPLIERS_FILE);

            CLI::clearScreen();
            CLI::printHeader("SYSTEM SHUTDOWN", CLI::BOLD_GREEN);
            std::cout << "\n   Inventory and supplier data stored securely in data/.\n";
            std::cout << "   Thank you for using Kirana Billing System. Goodbye!\n\n";
            CLI::printLine('=', 70, CLI::BOLD_GREEN);
            break;
        }
    }
    return 0;
}
