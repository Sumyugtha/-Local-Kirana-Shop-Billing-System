**🛒 Local Kirana Shop Billing System**

A modern, robust console-based billing, inventory, and supplier tracking application built in C++. Specially designed for small Indian retailers (Kirana stores) to digitize their paper-based invoicing, monitor real-time stock levels, receive low-stock alerts with supplier contact information, and audit daily sales and revenue metrics.


**🌟 Key Features**

**Smart Customer Invoicing:**
Create interactive sales sessions with optional customer detail tracking.
Auto-apply polymorphic category discounts (flat vs. bulk deals).
Perform real-time stock validation during item entry.
Decrement inventory upon checkout and prompt with red alerts for newly low-stocked items.

**Persistent Inventory Management:**
Read and write products to a text-based database file (data/inventory.txt).
Categorize items polymorphically as Grocery (5% flat discount), Snack (10% flat discount), or Beverage (8% volume discount on ≥5 units, 2% flat otherwise).
Search stock by ID or Name. Add/Remove items and adjust stock levels (restocking/correction) interactively.

**Supplier Tracking:**
Record distributor contact info (Name, contact person, phone, email) to data/suppliers.txt.
Automatically detect if a new product has an unregistered supplier, prompting immediate registration.
View specific distributor portfolios listing all items supplied, their current stock status, and reorder alerts.

**Interactive Reports & Analytics:**
Generate detailed daily sales summaries (collected gross sales, net revenue, total savings, and item counts).
Categorized sales breakdowns to evaluate what sells the most.
Search sales records by specific historical dates (YYYY-MM-DD).

**High-End Console UX:**
Customized ANSI escape sequences bring color highlights, warning cards, and boxed panels to standard terminal screens.
🏗️ Technical C++ Architecture (OOP Principles)

**Inheritance & Polymorphism:**
An abstract base class Item contains common properties (ID, name, base price, stock quantity, low-stock threshold, and supplier name).
Subclasses Grocery, Snack, and Beverage inherit from Item.
virtual double getDiscountedPrice(int purchaseQty) const = 0 calculates specific pricing on checkout based on the category's discount logic.
virtual std::string getCategoryName() const = 0 classifies items dynamically.

**Operator Overloading:**
friend std::ostream& operator<<(std::ostream&, const Item&): Overloaded to print item records in a clean, tabular format.
friend std::istream& operator>>(std::istream&, Item&): Overloaded to prompt and parse standard inputs sequentially.
friend std::ostream& operator<<(std::ostream&, const Bill&): Overloaded to render a stunning retail receipt on console with itemized totals and discount highlights.

**Data Streams & Text Databases:**
Standard file streams (std::ifstream and std::ofstream) manage database files using delimited parsing (|), ensuring the app remains fully zero-dependency.

**📂 Project Structure**

<img width="710" height="742" alt="image" src="https://github.com/user-attachments/assets/97fd09bf-8b8d-471a-85c5-70e6f4bbb561" />


📦**Mock Data (Ready out of the box!)**
We've prepopulated the databases under data/ with realistic local Indian retail data:

Suppliers: Amul Distributors, Tata Consumer Products, Britannia Foods Agency, Pepsico India Bottlers.
Inventory: Tata Salt, Britannia Marie Gold, Amul Cheese Block, Pepsi 2L Bottles, and more.
Alerts: Launching the system will immediately flag a low stock warning for the Amul Cheese Block and Pepsi bottles, allowing you to test the reorder alert feature immediately!
