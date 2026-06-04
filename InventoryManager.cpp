#include "../include/InventoryManager.h"
#include "../include/Grocery.h"
#include "../include/Snack.h"
#include "../include/Beverage.h"
#include <fstream>
#include <sstream>
#include <algorithm>

InventoryManager::InventoryManager() : nextId(101) {}

std::vector<std::string> InventoryManager::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool InventoryManager::loadInventory(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    items.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = split(line, '|');
        if (tokens.size() < 7) continue;

        std::string type = tokens[0];
        int id = std::stoi(tokens[1]);
        std::string name = tokens[2];
        double price = std::stod(tokens[3]);
        int qty = std::stoi(tokens[4]);
        int thresh = std::stoi(tokens[5]);
        std::string supplier = tokens[6];

        std::unique_ptr<Item> item = nullptr;
        if (type == "Grocery") {
            item = std::make_unique<Grocery>(id, name, price, qty, thresh, supplier);
        } else if (type == "Snack") {
            item = std::make_unique<Snack>(id, name, price, qty, thresh, supplier);
        } else if (type == "Beverage") {
            item = std::make_unique<Beverage>(id, name, price, qty, thresh, supplier);
        }

        if (item) {
            items.push_back(std::move(item));
            if (id >= nextId) {
                nextId = id + 1;
            }
        }
    }
    file.close();
    return true;
}

bool InventoryManager::saveInventory(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& item : items) {
        file << item->getCategoryName() << "|"
             << item->getId() << "|"
             << item->getName() << "|"
             << item->getPrice() << "|"
             << item->getQuantity() << "|"
             << item->getThreshold() << "|"
             << item->getSupplierName() << "\n";
    }
    file.close();
    return true;
}

void InventoryManager::addItem(std::unique_ptr<Item> item) {
    if (item->getId() == 0) {
        item->setId(nextId++);
    } else if (item->getId() >= nextId) {
        nextId = item->getId() + 1;
    }
    items.push_back(std::move(item));
}

bool InventoryManager::removeItem(int id) {
    auto it = std::remove_if(items.begin(), items.end(),
       [id](const std::unique_ptr<Item>& item) { return item->getId() == id; });

    if (it != items.end()) {
        items.erase(it, items.end());
        return true;
    }
    return false;
}

Item* InventoryManager::findItemById(int id) const {
    for (const auto& item : items) {
        if (item->getId() == id) {
            return item.get();
        }
    }
    return nullptr;
}

Item* InventoryManager::findItemByName(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& item : items) {
        std::string itemLower = item->getName();
        std::transform(itemLower.begin(), itemLower.end(), itemLower.begin(), ::tolower);

        if (itemLower == lowerName) {
            return item.get();
        }
    }
    return nullptr;
}

std::vector<Item*> InventoryManager::getLowStockItems() const {
    std::vector<Item*> lowStock;
    for (const auto& item : items) {
        if (item->isLowStock()) {
            lowStock.push_back(item.get());
        }
    }
    return lowStock;
}

bool InventoryManager::loadSuppliers(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    suppliers.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> tokens = split(line, '|');
        if (tokens.size() < 4) continue;

        suppliers.emplace_back(tokens[0], tokens[1], tokens[2], tokens[3]);
    }
    file.close();
    return true;
}

bool InventoryManager::saveSuppliers(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& sup : suppliers) {
        file << sup.getName() << "|"
             << sup.getContactPerson() << "|"
             << sup.getPhone() << "|"
             << sup.getEmail() << "\n";
    }
    file.close();
    return true;
}

void InventoryManager::addSupplier(const Supplier& supplier) {
    for (const auto& s : suppliers) {
        if (s.getName() == supplier.getName()) {
            return;
        }
    }
    suppliers.push_back(supplier);
}

const Supplier* InventoryManager::findSupplierByName(const std::string& name) const {
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

    for (const auto& sup : suppliers) {
        std::string supLower = sup.getName();
        std::transform(supLower.begin(), supLower.end(), supLower.begin(), ::tolower);
        if (supLower == lowerName) {
            return &sup;
        }
    }
    return nullptr;
}
