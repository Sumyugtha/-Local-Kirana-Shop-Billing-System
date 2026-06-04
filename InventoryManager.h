#ifndef INVENTORY_MANAGER_H
#define INVENTORY_MANAGER_H

#include "Item.h"
#include "Supplier.h"
#include <vector>
#include <memory>
#include <string>

class InventoryManager {
private:
    std::vector<std::unique_ptr<Item>> items;
    std::vector<Supplier> suppliers;
    int nextId;

    std::vector<std::string> split(const std::string& str, char delimiter) const;

public:
    InventoryManager();

    bool loadInventory(const std::string& filename);
    bool saveInventory(const std::string& filename);
    void addItem(std::unique_ptr<Item> item);
    bool removeItem(int id);
    Item* findItemById(int id) const;
    Item* findItemByName(const std::string& name) const;
    std::vector<Item*> getLowStockItems() const;
    const std::vector<std::unique_ptr<Item>>& getItems() const { return items; }
    int getNextId() const { return nextId; }

    bool loadSuppliers(const std::string& filename);
    bool saveSuppliers(const std::string& filename);
    void addSupplier(const Supplier& supplier);
    const Supplier* findSupplierByName(const std::string& name) const;
    const std::vector<Supplier>& getSuppliers() const { return suppliers; }
};

#endif