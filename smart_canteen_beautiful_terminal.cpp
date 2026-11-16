#include <iostream>
#include <iomanip>
#include <string>
#include <limits>
#include <queue>
#include <stack>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <array>
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdio>
#include <vector>
#include <memory>
#include <functional>

using namespace std;

// ============================================================
// CANTEEN QUEUE & TOKEN MANAGEMENT SYSTEM
// C++17 | OOPS + DSA integrated console project
// Includes persistence, staff tools, analytics, feedback, and export modules.
// ============================================================

const int MENU_SIZE = 8; // Fixed starter catalogue; items can be edited by staff.
const int MAX_ORDERS = 1000;
const int NORMAL_QUEUE_SIZE = 1000;

// ------------------------- Utility --------------------------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int readInt(const string& prompt, int minValue = numeric_limits<int>::min(),
            int maxValue = numeric_limits<int>::max()) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= minValue && value <= maxValue) {
            clearInput();
            return value;
        }
        cout << "Invalid input. Please enter a valid number.\n";
        clearInput();
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

string toLower(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return value;
}

// ---------------------- OOPS: FoodItem -----------------------
// Encapsulation: data members are private; access is controlled by methods.
class FoodItem {
private:
    int id;
    string name;
    string category;
    double price;
    int quantity;

public:
    FoodItem() : id(0), name(""), category(""), price(0.0), quantity(0) {}

    FoodItem(int itemId, const string& itemName, const string& itemCategory,
             double itemPrice, int itemQuantity)
        : id(itemId), name(itemName), category(itemCategory),
          price(itemPrice), quantity(itemQuantity) {}

    int getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }

    void setQuantity(int newQuantity) {
        if (newQuantity >= 0) quantity = newQuantity;
    }

    bool reduceStock(int amount) {
        if (amount <= 0 || amount > quantity) return false;
        quantity -= amount;
        return true;
    }

    void addStock(int amount) {
        if (amount > 0) quantity += amount;
    }

    void display() const {
        cout << left << setw(6) << id << setw(20) << name
             << setw(14) << category << "Rs. " << setw(9)
             << fixed << setprecision(2) << price << quantity << '\n';
    }
};

// ----------------------- OOPS: Order -------------------------
enum class OrderStatus { Pending, Preparing, Ready, Collected, Cancelled };

string statusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::Pending: return "Pending";
        case OrderStatus::Preparing: return "Preparing";
        case OrderStatus::Ready: return "Ready";
        case OrderStatus::Collected: return "Collected";
        case OrderStatus::Cancelled: return "Cancelled";
    }
    return "Unknown";
}

class Order {
private:
    int token;
    int foodId;
    string foodName;
    int quantity;
    double bill;
    string paymentMethod;
    bool priorityOrder;
    int priorityLevel;
    OrderStatus status;

public:
    Order()
        : token(0), foodId(0), quantity(0), bill(0.0),
          priorityOrder(false), priorityLevel(1), status(OrderStatus::Pending) {}

    Order(int orderToken, int itemId, const string& itemName, int itemQuantity,
          double finalBill, const string& payment, bool isPriority, int priority)
        : token(orderToken), foodId(itemId), foodName(itemName),
          quantity(itemQuantity), bill(finalBill), paymentMethod(payment),
          priorityOrder(isPriority), priorityLevel(priority),
          status(OrderStatus::Pending) {}

    int getToken() const { return token; }
    int getFoodId() const { return foodId; }
    string getFoodName() const { return foodName; }
    int getQuantity() const { return quantity; }
    double getBill() const { return bill; }
    string getPaymentMethod() const { return paymentMethod; }
    bool isPriority() const { return priorityOrder; }
    int getPriorityLevel() const { return priorityLevel; }
    OrderStatus getStatus() const { return status; }
    void setStatus(OrderStatus newStatus) { status = newStatus; }

    void display() const {
        cout << "Token: " << token << " | Food: " << foodName
             << " | Qty: " << quantity << " | Bill: Rs. "
             << fixed << setprecision(2) << bill
             << " | Payment: " << paymentMethod
             << " | Type: " << (priorityOrder ? "Priority" : "Normal")
             << " | Status: " << statusToString(status) << '\n';
    }
};

// ---------------- OOPS: Inheritance + Polymorphism ------------
class User {
protected:
    int userId;
    string name;

public:
    User(int id, const string& userName) : userId(id), name(userName) {}
    virtual void showRole() const = 0; // Abstraction: pure virtual function
    virtual ~User() = default;
};

class Customer : public User {
public:
    Customer(int id, const string& customerName) : User(id, customerName) {}
    void showRole() const override {
        cout << "Customer: " << name << " (ID: " << userId << ")\n";
    }
};

class Staff : public User {
public:
    Staff(int id, const string& staffName) : User(id, staffName) {}
    void showRole() const override {
        cout << "Staff: " << name << " (ID: " << userId << ")\n";
    }
};

// ---------------- OOPS: Payment Abstraction ------------------
class PaymentMethod {
public:
    virtual string pay(double amount) const = 0;
    virtual ~PaymentMethod() = default;
};

class CashPayment : public PaymentMethod {
public:
    string pay(double amount) const override {
        (void)amount;
        return "Cash";
    }
};

class UPIPayment : public PaymentMethod {
public:
    string pay(double amount) const override {
        (void)amount;
        return "UPI (simulated)";
    }
};

class CardPayment : public PaymentMethod {
public:
    string pay(double amount) const override {
        (void)amount;
        return "Card (simulated)";
    }
};

// ---------------------- DSA: Circular Queue ------------------
// Stores tokens of normal orders and follows FIFO.
class CircularQueue {
private:
    int data[NORMAL_QUEUE_SIZE];
    int frontIndex;
    int rearIndex;
    int count;

public:
    CircularQueue() : frontIndex(0), rearIndex(-1), count(0) {}

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == NORMAL_QUEUE_SIZE; }

    bool enqueue(int token) {
        if (isFull()) return false;
        rearIndex = (rearIndex + 1) % NORMAL_QUEUE_SIZE;
        data[rearIndex] = token;
        ++count;
        return true;
    }

    bool dequeue(int& token) {
        if (isEmpty()) return false;
        token = data[frontIndex];
        frontIndex = (frontIndex + 1) % NORMAL_QUEUE_SIZE;
        --count;
        return true;
    }

    int size() const { return count; }
};

// ---------------- DSA: Stack record for Undo -----------------
struct CancelledRecord {
    int orderIndex;
    Order previousOrder;
};

// -------------------- Extra OOPS: Coupon ---------------------
// A small domain class keeps discount rules separate from order logic.
class Coupon {
private:
    string code;
    double discountPercent;
    double minimumBill;
    bool active;
public:
    Coupon() : code("SAVE10"), discountPercent(10.0), minimumBill(0.0), active(true) {}
    Coupon(const string& c, double d, double m, bool a)
        : code(c), discountPercent(d), minimumBill(m), active(a) {}
    string getCode() const { return code; }
    double getDiscountPercent() const { return discountPercent; }
    bool isActive() const { return active; }
    void setActive(bool value) { active = value; }
    bool apply(const string& entered, double& bill) const {
        if (!active || toLower(entered) != toLower(code) || bill < minimumBill) return false;
        bill -= bill * discountPercent / 100.0;
        return true;
    }
};

// ------------------- Extra OOPS: Feedback --------------------
class Feedback {
private:
    int token;
    int rating;
    string comment;
public:
    Feedback() : token(0), rating(0), comment("") {}
    Feedback(int t, int r, const string& c) : token(t), rating(r), comment(c) {}
    int getToken() const { return token; }
    int getRating() const { return rating; }
    string getComment() const { return comment; }
    void display() const {
        cout << "Token " << token << " | Rating: " << rating << "/5 | " << comment << '\n';
    }
};

// ---------------------- Main System Class --------------------
class CanteenSystem {
private:
    array<FoodItem, MENU_SIZE> menu;
    array<Order, MAX_ORDERS> orders;
    int orderCount;
    int nextToken;

    CircularQueue normalQueue;
    // pair: (priority level, token); larger priority level is served first.
    priority_queue<pair<int, int>> priorityOrders;
    stack<CancelledRecord> cancellationHistory;
    unordered_map<int, int> tokenIndex; // Hashing: token -> order array index

    double totalWasteValue;
    Coupon activeCoupon;
    vector<Feedback> feedbackRecords;
    int lowStockThreshold;
    bool staffAuthenticated;

    int findOrderLinear(int token) const {
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].getToken() == token) return i;
        }
        return -1;
    }

    bool isActivePending(int index) const {
        return index >= 0 && index < orderCount &&
               orders[index].getStatus() == OrderStatus::Pending;
    }

    void cleanPriorityQueue() {
        while (!priorityOrders.empty()) {
            int token = -priorityOrders.top().second;
            auto it = tokenIndex.find(token);
            if (it != tokenIndex.end() && isActivePending(it->second)) break;
            priorityOrders.pop(); // Remove stale/cancelled/already processed entry.
        }
    }

    bool getNextNormalToken(int& token) {
        while (normalQueue.dequeue(token)) {
            auto it = tokenIndex.find(token);
            if (it != tokenIndex.end() && isActivePending(it->second)) return true;
            // A cancelled or already processed token is skipped.
        }
        return false;
    }

    int findFoodLinear(int foodId) const {
        for (int i = 0; i < MENU_SIZE; ++i) {
            if (menu[i].getId() == foodId) return i;
        }
        return -1;
    }

    // Binary Search requires menu to remain sorted by ID.
    int findFoodBinary(int foodId) const {
        int low = 0, high = MENU_SIZE - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (menu[mid].getId() == foodId) return mid;
            if (menu[mid].getId() < foodId) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }

    string currentDateTime() const {
        time_t now = time(nullptr);
        tm* local = localtime(&now);
        if (!local) return "Unknown time";
        ostringstream out;
        out << put_time(local, "%Y-%m-%d %H:%M:%S");
        return out.str();
    }

    bool requireStaffAccess() const {
        if (staffAuthenticated) return true;
        cout << "Staff access required. Please login from the main menu first.\n";
        return false;
    }

    void logActivity(const string& message) const {
        ofstream log("canteen_activity.log", ios::app);
        if (log) log << currentDateTime() << " | " << message << '\n';
    }

    const PaymentMethod& choosePayment(int choice, CashPayment& cash,
                                       UPIPayment& upi, CardPayment& card) const {
        if (choice == 1) return cash;
        if (choice == 2) return upi;
        return card;
    }

public:
    CanteenSystem() : orderCount(0), nextToken(101), totalWasteValue(0.0),
        activeCoupon(), lowStockThreshold(5), staffAuthenticated(false) {
        // Menu is initialized in ascending ID order for Binary Search.
        menu[0] = FoodItem(1, "Samosa", "Snacks", 15.0, 30);
        menu[1] = FoodItem(2, "Sandwich", "Snacks", 50.0, 20);
        menu[2] = FoodItem(3, "Burger", "Fast Food", 70.0, 15);
        menu[3] = FoodItem(4, "Tea", "Beverage", 10.0, 40);
        menu[4] = FoodItem(5, "Cold Coffee", "Beverage", 60.0, 12);
        menu[5] = FoodItem(6, "Veg Roll", "Snacks", 45.0, 18);
        menu[6] = FoodItem(7, "Pizza Slice", "Fast Food", 80.0, 10);
        menu[7] = FoodItem(8, "Lemonade", "Beverage", 25.0, 25);
        ifstream existingData("canteen_data.txt");
        bool firstLaunch = !existingData.good();
        existingData.close();
        loadData();

        // Upgrade only the exact earlier four-order demo seed. Any larger
        // saved history belongs to the user and is left untouched.
        bool oldDemoSeed = orderCount == 4 &&
            orders[0].getToken() == 101 && orders[1].getToken() == 102 &&
            orders[2].getToken() == 103 && orders[3].getToken() == 104;

        if ((firstLaunch && orderCount == 0) || oldDemoSeed) {
            orderCount = 10;
            nextToken = 111;
            totalWasteValue = 35.0;

            orders[0] = Order(101, 2, "Sandwich", 2, 100.0, "UPI (simulated)", false, 1);
            orders[0].setStatus(OrderStatus::Collected);
            orders[1] = Order(102, 3, "Burger", 1, 70.0, "Cash", true, 3);
            orders[2] = Order(103, 4, "Tea", 2, 20.0, "Cash", false, 1);
            orders[3] = Order(104, 6, "Veg Roll", 1, 45.0, "Card (simulated)", false, 1);
            orders[3].setStatus(OrderStatus::Ready);
            orders[4] = Order(105, 1, "Samosa", 3, 45.0, "Cash", false, 1);
            orders[4].setStatus(OrderStatus::Collected);
            orders[5] = Order(106, 5, "Cold Coffee", 1, 60.0, "UPI (simulated)", false, 1);
            orders[5].setStatus(OrderStatus::Preparing);
            orders[6] = Order(107, 7, "Pizza Slice", 2, 160.0, "Card (simulated)", false, 1);
            orders[7] = Order(108, 8, "Lemonade", 2, 50.0, "UPI (simulated)", false, 1);
            orders[7].setStatus(OrderStatus::Collected);
            orders[8] = Order(109, 2, "Sandwich", 1, 50.0, "Cash", false, 1);
            orders[8].setStatus(OrderStatus::Cancelled);
            orders[9] = Order(110, 3, "Burger", 2, 140.0, "UPI (simulated)", false, 1);
            orders[9].setStatus(OrderStatus::Collected);

            // Inventory balances after the demo sales and active orders.
            menu[0].setQuantity(27); // Samosa
            menu[1].setQuantity(18); // Sandwich
            menu[2].setQuantity(12); // Burger
            menu[3].setQuantity(38); // Tea
            menu[4].setQuantity(11); // Cold Coffee
            menu[5].setQuantity(17); // Veg Roll
            menu[6].setQuantity(8);  // Pizza Slice
            menu[7].setQuantity(23); // Lemonade

            tokenIndex.clear();
            while (!priorityOrders.empty()) priorityOrders.pop();
            int ignoredToken = -1;
            while (normalQueue.dequeue(ignoredToken)) {}
            for (int i = 0; i < orderCount; ++i) {
                int token = orders[i].getToken();
                tokenIndex[token] = i;
                if (orders[i].getStatus() == OrderStatus::Pending) {
                    if (orders[i].isPriority())
                        priorityOrders.push({orders[i].getPriorityLevel(), -token});
                    else
                        normalQueue.enqueue(token);
                }
            }

            feedbackRecords.clear();
            feedbackRecords.emplace_back(101, 5, "Fresh sandwich and quick service.");
            feedbackRecords.emplace_back(105, 4, "Samosas were crispy and hot.");
            feedbackRecords.emplace_back(108, 5, "Refreshing lemonade. Good experience.");
            saveData();
        } else if (firstLaunch) {
            saveData();
        }
    }

    void showWelcomeOverview() const {
        int pending = 0, preparing = 0, ready = 0, collected = 0;
        double revenue = 0.0;
        int stockUnits = 0;
        for (int i = 0; i < orderCount; ++i) {
            switch (orders[i].getStatus()) {
                case OrderStatus::Pending: ++pending; break;
                case OrderStatus::Preparing: ++preparing; break;
                case OrderStatus::Ready: ++ready; break;
                case OrderStatus::Collected: ++collected; revenue += orders[i].getBill(); break;
                case OrderStatus::Cancelled: break;
            }
        }
        for (const auto& item : menu) stockUnits += item.getQuantity();

        cout << string(84, '=') << '\n';
        cout << "\033[1;36m";
        cout << "                 WELCOME TO SMART CANTEEN\n";
        cout << "\033[0m";
        cout << string(84, '-') << '\n';
        cout << "  STAFF ON DUTY       : Aarav Mehta  | Staff ID: STF-9001\n";
        cout << "  ROLE / SHIFT        : Canteen Manager | 09:00 AM - 05:00 PM\n";
        cout << "  COUNTER             : Counter 02     | Demo staff profile\n";
        cout << string(84, '-') << '\n';
        cout << "  TODAY'S SNAPSHOT    : " << orderCount << " preloaded demo/recorded orders"
             << " | " << stockUnits << " stock units\n";
        cout << "  ORDER STATUS        : Pending " << pending
             << " | Preparing " << preparing << " | Ready " << ready
             << " | Collected " << collected << "\n";
        cout << "  RECORDED SALES      : Rs. " << fixed << setprecision(2) << revenue << "\n";
        cout << "  STAFF LOGIN         : username: staff | demo PIN: 2468\n";
        cout << string(84, '-') << '\n';
        cout << "  PERSISTENCE DEMO    : 1) Customer Services > 5: place an order (repeat twice)\n";
        cout << "                        2) Order Management > 9: view all saved orders\n";
        cout << "                        3) Choose 0 to save & exit, then run this app again\n";
        cout << "                        4) Open Order Management > 9: both new orders remain\n";
        cout << string(84, '=') << '\n';
        cout << "\033[0;90m  First run includes sample records for demonstration. "
                "New orders are saved locally after each successful order.\033[0m\n\n";
    }

    void showMenu() const {
        cout << "\n================ CANTEEN MENU ================\n";
        cout << left << setw(6) << "ID" << setw(20) << "Food"
             << setw(14) << "Category" << setw(14) << "Price" << "Stock\n";
        cout << "---------------------------------------------------------------\n";
        for (const auto& item : menu) item.display();
    }

    void searchFood() const {
        cout << "\n1. Search by Linear Search\n2. Search by Binary Search (ID)\n";
        int method = readInt("Choose search method: ", 1, 2);
        int id = readInt("Enter food ID: ", 1);
        int index = (method == 1) ? findFoodLinear(id) : findFoodBinary(id);
        if (index == -1) cout << "Food item not found.\n";
        else {
            cout << "Food found: ";
            menu[index].display();
        }
    }

    void searchFoodByName() const {
        string query = toLower(readLine("Enter food name or part of name: "));
        bool found = false;
        for (const auto& item : menu) {
            if (toLower(item.getName()).find(query) != string::npos) {
                item.display();
                found = true;
            }
        }
        if (!found) cout << "No matching food item found.\n";
    }

    void displayMenuSortedByPrice() const {
        // Bubble Sort on a copy; original ID-sorted menu remains unchanged.
        array<FoodItem, MENU_SIZE> sortedMenu = menu;
        for (int i = 0; i < MENU_SIZE - 1; ++i) {
            for (int j = 0; j < MENU_SIZE - i - 1; ++j) {
                if (sortedMenu[j].getPrice() > sortedMenu[j + 1].getPrice())
                    swap(sortedMenu[j], sortedMenu[j + 1]);
            }
        }
        cout << "\nFood items sorted by price (low to high):\n";
        for (const auto& item : sortedMenu) item.display();
    }

    void showInventory() const {
        cout << "\n================ INVENTORY ===================\n";
        for (const auto& item : menu) {
            cout << left << setw(20) << item.getName()
                 << "Stock: " << item.getQuantity() << '\n';
        }
    }

    void restockItem() {
        int id = readInt("Enter food ID to restock: ", 1);
        int index = findFoodBinary(id);
        if (index == -1) {
            cout << "Food item not found.\n";
            return;
        }
        int amount = readInt("Enter quantity to add: ", 1);
        menu[index].addStock(amount);
        cout << "Stock updated successfully. New stock: "
             << menu[index].getQuantity() << '\n';
        logActivity("Stock restocked for item " + to_string(id));
        saveData();
    }

    void recordWaste() {
        int id = readInt("Enter food ID: ", 1);
        int index = findFoodBinary(id);
        if (index == -1) {
            cout << "Food item not found.\n";
            return;
        }
        int amount = readInt("Enter wasted quantity: ", 1);
        if (amount > menu[index].getQuantity()) {
            cout << "Waste quantity cannot exceed available stock.\n";
            return;
        }
        menu[index].reduceStock(amount);
        double value = amount * menu[index].getPrice();
        totalWasteValue += value;
        cout << "Waste recorded. Estimated waste value: Rs. "
             << fixed << setprecision(2) << value << '\n';
        logActivity("Food waste recorded for item " + to_string(id));
        saveData();
    }

    void placeOrder() {
        if (orderCount >= MAX_ORDERS) {
            cout << "Order storage is full.\n";
            return;
        }

        Customer customer(1, "Canteen Customer");
        customer.showRole();

        int foodId = readInt("Enter food ID: ", 1);
        int foodIndex = findFoodBinary(foodId);
        if (foodIndex == -1) {
            cout << "Invalid food ID.\n";
            return;
        }

        int quantity = readInt("Enter quantity: ", 1);
        if (quantity > menu[foodIndex].getQuantity()) {
            cout << "Insufficient stock. Available: "
                 << menu[foodIndex].getQuantity() << '\n';
            return;
        }

        int orderType = readInt("Order type: 1. Normal  2. Faculty/Priority: ", 1, 2);
        bool isPriority = (orderType == 2);
        int priorityLevel = isPriority ? 3 : 1;

        double bill = menu[foodIndex].getPrice() * quantity;
        string couponCode = readLine("Enter coupon code (or press Enter to skip): ");
        if (!couponCode.empty()) {
            if (activeCoupon.apply(couponCode, bill))
                cout << "Coupon applied successfully.\n";
            else
                cout << "Coupon is invalid, inactive, or bill does not meet its condition.\n";
        }

        cout << "Payment: 1. Cash  2. UPI  3. Card\n";
        int paymentChoice = readInt("Choose payment method: ", 1, 3);
        CashPayment cash;
        UPIPayment upi;
        CardPayment card;
        const PaymentMethod& payment = choosePayment(paymentChoice, cash, upi, card);
        string paymentName = payment.pay(bill);

        int token = nextToken;
        Order newOrder(token, foodId, menu[foodIndex].getName(), quantity,
                       bill, paymentName, isPriority, priorityLevel);

        // Save order and stock update only after all inputs are validated.
        orders[orderCount] = newOrder;
        tokenIndex[token] = orderCount;
        menu[foodIndex].reduceStock(quantity);

        bool queued = isPriority
            ? (priorityOrders.push({priorityLevel, -token}), true)
            : normalQueue.enqueue(token);

        if (!queued) {
            menu[foodIndex].addStock(quantity);
            tokenIndex.erase(token);
            cout << "Queue is full. Order was not accepted.\n";
            return;
        }

        cout << "\nOrder placed successfully!\n";
        orders[orderCount].display();
        ++orderCount;
        ++nextToken;
        logActivity("New order placed: token " + to_string(token));
        saveData();
        cout << "\033[1;32m[DATA SAVED]\033[0m Order " << token
             << " has been saved to canteen_data.txt. It will be available after restart.\n";
    }

    void processNextOrder() {
        cleanPriorityQueue();
        int token = -1;
        int index = -1;

        // Explicit rule: valid priority orders are served before normal FIFO orders.
        if (!priorityOrders.empty()) {
            token = -priorityOrders.top().second;
            priorityOrders.pop();
            auto it = tokenIndex.find(token);
            if (it != tokenIndex.end()) index = it->second;
        } else {
            if (!getNextNormalToken(token)) {
                cout << "No pending orders in the queue.\n";
                return;
            }
            auto it = tokenIndex.find(token);
            if (it != tokenIndex.end()) index = it->second;
        }

        if (!isActivePending(index)) {
            cout << "No valid pending order found.\n";
            return;
        }

        orders[index].setStatus(OrderStatus::Preparing);
        logActivity("Order started: token " + to_string(token));
        saveData();
        cout << "Now preparing order: ";
        orders[index].display();
    }

    void updateOrderStatus() {
        int token = readInt("Enter token number: ", 1);
        int index = findOrderLinear(token);
        if (index == -1) {
            cout << "Order not found.\n";
            return;
        }

        cout << "Current status: " << statusToString(orders[index].getStatus()) << '\n';
        cout << "1. Preparing\n2. Ready\n3. Collected\n";
        int choice = readInt("Select new status: ", 1, 3);

        OrderStatus current = orders[index].getStatus();
        if (current == OrderStatus::Cancelled || current == OrderStatus::Collected) {
            cout << "This order cannot be updated further.\n";
            return;
        }
        OrderStatus requested = static_cast<OrderStatus>(choice);
        if (static_cast<int>(requested) != static_cast<int>(current) + 1) {
            cout << "Invalid transition. Move the order one step at a time: "
                 << statusToString(current) << " -> "
                 << statusToString(static_cast<OrderStatus>(static_cast<int>(current) + 1))
                 << ".\n";
            return;
        }
        orders[index].setStatus(requested);

        cout << "Order status updated to "
             << statusToString(orders[index].getStatus()) << ".\n";
        logActivity("Order status changed: token " + to_string(token));
        saveData();
    }

    void searchOrderByToken() const {
        int token = readInt("Enter order token: ", 1);
        // Hash lookup gives average O(1) access to the order array index.
        auto it = tokenIndex.find(token);
        if (it == tokenIndex.end()) {
            cout << "No order found for this token.\n";
            return;
        }
        orders[it->second].display();
    }

    void showAllOrders() const {
        if (orderCount == 0) {
            cout << "No orders have been placed yet.\n";
            return;
        }
        cout << "\n================ ALL ORDERS =================\n";
        for (int i = 0; i < orderCount; ++i) orders[i].display();
    }

    void cancelOrder() {
        int token = readInt("Enter token to cancel: ", 1);
        int index = findOrderLinear(token);
        if (index == -1) {
            cout << "Order not found.\n";
            return;
        }
        OrderStatus status = orders[index].getStatus();
        if (status == OrderStatus::Collected || status == OrderStatus::Cancelled) {
            cout << "Collected or already cancelled orders cannot be cancelled.\n";
            return;
        }

        int foodIndex = findFoodBinary(orders[index].getFoodId());
        if (foodIndex == -1) {
            cout << "Food record not found; cancellation aborted.\n";
            return;
        }

        CancelledRecord record;
        record.orderIndex = index;
        record.previousOrder = orders[index];
        cancellationHistory.push(record); // Stack: latest cancellation is undone first.

        menu[foodIndex].addStock(orders[index].getQuantity());
        orders[index].setStatus(OrderStatus::Cancelled);
        cout << "Order cancelled. Token " << token
             << " saved in undo history.\n";
        logActivity("Order cancelled: token " + to_string(token));
        saveData();
    }

    void undoCancellation() {
        if (cancellationHistory.empty()) {
            cout << "No cancellation available to undo.\n";
            return;
        }

        CancelledRecord record = cancellationHistory.top();
        int index = record.orderIndex;
        int foodIndex = findFoodBinary(record.previousOrder.getFoodId());
        if (foodIndex == -1) {
            cout << "Food record missing; cannot restore order.\n";
            return;
        }
        if (menu[foodIndex].getQuantity() < record.previousOrder.getQuantity()) {
            cout << "Not enough stock to restore this order. Restock first.\n";
            return;
        }

        cancellationHistory.pop();
        menu[foodIndex].reduceStock(record.previousOrder.getQuantity());
        orders[index] = record.previousOrder;
        orders[index].setStatus(OrderStatus::Pending);

        bool queued = orders[index].isPriority()
            ? (priorityOrders.push({orders[index].getPriorityLevel(), -orders[index].getToken()}), true)
            : normalQueue.enqueue(orders[index].getToken());

        if (!queued) {
            // Roll back if the normal queue cannot accept the restored order.
            menu[foodIndex].addStock(record.previousOrder.getQuantity());
            orders[index].setStatus(OrderStatus::Cancelled);
            cancellationHistory.push(record);
            cout << "Queue is full; undo could not be completed.\n";
            return;
        }
        cout << "Cancellation undone. Token " << orders[index].getToken()
             << " returned to the pending queue.\n";
        logActivity("Cancellation undone: token " + to_string(orders[index].getToken()));
        saveData();
    }

    void showQueueSummary() {
        cleanPriorityQueue();
        cout << "\n================ QUEUE SUMMARY ===============\n";
        cout << "Normal queue entries (may include cancelled entries awaiting skip): "
             << normalQueue.size() << '\n';
        cout << "Priority queue valid top: ";
        if (priorityOrders.empty()) cout << "None\n";
        else cout << "Token " << -priorityOrders.top().second
                  << " (priority " << priorityOrders.top().first << ")\n";
        int pending = 0;
        for (int i = 0; i < orderCount; ++i)
            if (orders[i].getStatus() == OrderStatus::Pending) ++pending;
        cout << "Pending orders in records: " << pending << '\n';
    }

    void showStaffDashboard() const {
        int pending = 0, preparing = 0, ready = 0, collected = 0, cancelled = 0;
        double revenue = 0.0;
        for (int i = 0; i < orderCount; ++i) {
            switch (orders[i].getStatus()) {
                case OrderStatus::Pending: ++pending; break;
                case OrderStatus::Preparing: ++preparing; break;
                case OrderStatus::Ready: ++ready; break;
                case OrderStatus::Collected:
                    ++collected;
                    revenue += orders[i].getBill();
                    break;
                case OrderStatus::Cancelled: ++cancelled; break;
            }
        }

        cout << "\n================ STAFF DASHBOARD ================\n";
        cout << "STAFF PROFILE : Aarav Mehta | ID: STF-9001 | Manager\n";
        cout << "SHIFT         : 09:00 AM - 05:00 PM | COUNTER: 02\n";
        cout << "LOGIN         : " << (staffAuthenticated ? "ACTIVE" : "NOT LOGGED IN")
             << " | Demo credentials: staff / 2468\n";
        cout << "--------------------------------------------------\n";
        cout << "Total orders: " << orderCount << '\n';
        cout << "Pending: " << pending << '\n';
        cout << "Preparing: " << preparing << '\n';
        cout << "Ready: " << ready << '\n';
        cout << "Collected: " << collected << '\n';
        cout << "Cancelled: " << cancelled << '\n';
        cout << "Collected revenue: Rs. " << fixed << setprecision(2) << revenue << '\n';
        cout << "Estimated recorded food waste value: Rs. "
             << totalWasteValue << '\n';
        cout << "Approximate pending wait: " << pending * 5
             << " minutes (assumption: 5 minutes/order).\n";
    }

    void showDailyReport() const {
        double totalSales = 0.0;
        int completed = 0;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].getStatus() == OrderStatus::Collected) {
                totalSales += orders[i].getBill();
                ++completed;
            }
        }
        cout << "\n================ DAILY REPORT ================\n";
        cout << "Orders recorded: " << orderCount << '\n';
        cout << "Completed orders: " << completed << '\n';
        cout << "Total collected sales: Rs. " << fixed << setprecision(2)
             << totalSales << '\n';
        cout << "Estimated food waste value: Rs. " << totalWasteValue << '\n';
    }

    // -------------------- Persistence Module -----------------
    // Text-file storage is intentionally simple for a classroom console project.
    // Files are created in the directory from which the program is launched.
    void saveData() const {
        ofstream file("canteen_data.txt");
        if (!file) {
            cout << "Warning: could not save canteen_data.txt.\n";
            return;
        }
        file << "CANTEEN_V1\n";
        file << orderCount << ' ' << nextToken << ' ' << totalWasteValue << '\n';
        for (const auto& item : menu) {
            file << item.getId() << '|' << item.getName() << '|' << item.getCategory()
                 << '|' << item.getPrice() << '|' << item.getQuantity() << '\n';
        }
        for (int i = 0; i < orderCount; ++i) {
            const Order& o = orders[i];
            file << o.getToken() << '|' << o.getFoodId() << '|' << o.getFoodName()
                 << '|' << o.getQuantity() << '|' << o.getBill() << '|'
                 << o.getPaymentMethod() << '|' << o.isPriority() << '|'
                 << o.getPriorityLevel() << '|' << static_cast<int>(o.getStatus()) << '\n';
        }
        file.close();
        saveFeedback();
        saveSeparateDataFiles();
    }

    // Create readable mirror files so each project data category is visible
    // separately during demonstration and viva. The master file above remains
    // the source used to restore the complete application state.
    void saveSeparateDataFiles() const {
        {
            ofstream menuFile("menu.txt");
            if (menuFile) {
                menuFile << "ID|NAME|CATEGORY|PRICE|STOCK\n";
                for (const auto& item : menu) {
                    menuFile << item.getId() << '|'
                             << item.getName() << '|'
                             << item.getCategory() << '|'
                             << fixed << setprecision(2) << item.getPrice() << '|'
                             << item.getQuantity() << '\n';
                }
            }
        }
        {
            ofstream orderFile("orders.txt");
            if (orderFile) {
                orderFile << "TOKEN|FOOD_ID|FOOD_NAME|QUANTITY|BILL|PAYMENT|PRIORITY|STATUS\n";
                for (int i = 0; i < orderCount; ++i) {
                    const Order& order = orders[i];
                    orderFile << order.getToken() << '|'
                              << order.getFoodId() << '|'
                              << order.getFoodName() << '|'
                              << order.getQuantity() << '|'
                              << fixed << setprecision(2) << order.getBill() << '|'
                              << order.getPaymentMethod() << '|'
                              << (order.isPriority() ? "YES" : "NO") << '|'
                              << statusToString(order.getStatus()) << '\n';
                }
            }
        }
        {
            ofstream tokenFile("tokens.txt");
            if (tokenFile) {
                tokenFile << "TOKEN|ORDER_STATUS|ORDER_TYPE\n";
                for (int i = 0; i < orderCount; ++i) {
                    tokenFile << orders[i].getToken() << '|'
                              << statusToString(orders[i].getStatus()) << '|'
                              << (orders[i].isPriority() ? "PRIORITY" : "NORMAL")
                              << '\n';
                }
            }
        }
        {
            ofstream inventoryFile("inventory.txt");
            if (inventoryFile) {
                inventoryFile << "FOOD_ID|FOOD_NAME|AVAILABLE_STOCK|UNIT_PRICE|STOCK_VALUE\n";
                for (const auto& item : menu) {
                    inventoryFile << item.getId() << '|'
                                  << item.getName() << '|'
                                  << item.getQuantity() << '|'
                                  << fixed << setprecision(2) << item.getPrice() << '|'
                                  << item.getPrice() * item.getQuantity() << '\n';
                }
                inventoryFile << "TOTAL_WASTE_VALUE|" << totalWasteValue << '\n';
            }
        }
    }

    void loadData() {
        ifstream file("canteen_data.txt");
        if (!file) return; // First run: use the built-in sample menu.
        string header;
        getline(file, header);
        if (header != "CANTEEN_V1") {
            cout << "Existing data format not recognised; using fresh sample data.\n";
            return;
        }
        int savedOrders = 0;
        double savedWaste = 0.0;
        if (!(file >> savedOrders >> nextToken >> savedWaste)) return;
        file.ignore(numeric_limits<streamsize>::max(), '\n');
        for (int i = 0; i < MENU_SIZE; ++i) {
            string line;
            if (!getline(file, line)) return;
            stringstream row(line);
            string id, name, category, price, stock;
            getline(row, id, '|'); getline(row, name, '|');
            getline(row, category, '|'); getline(row, price, '|'); getline(row, stock, '|');
            try {
                menu[i] = FoodItem(stoi(id), name, category, stod(price), stoi(stock));
            } catch (...) { return; }
        }
        orderCount = 0;
        for (int i = 0; i < savedOrders && i < MAX_ORDERS; ++i) {
            string line;
            if (!getline(file, line)) break;
            stringstream row(line);
            string fields[9];
            for (int j = 0; j < 9; ++j) getline(row, fields[j], '|');
            try {
                int token = stoi(fields[0]);
                int foodId = stoi(fields[1]);
                int quantity = stoi(fields[3]);
                double bill = stod(fields[4]);
                bool priority = (fields[6] == "1");
                int level = stoi(fields[7]);
                int statusNumber = stoi(fields[8]);
                if (statusNumber < 0 || statusNumber > 4) statusNumber = 0;
                Order restored(token, foodId, fields[2], quantity, bill, fields[5], priority, level);
                restored.setStatus(static_cast<OrderStatus>(statusNumber));
                orders[orderCount] = restored;
                tokenIndex[token] = orderCount;
                if (restored.getStatus() == OrderStatus::Pending) {
                    if (priority) priorityOrders.push({level, -token});
                    else normalQueue.enqueue(token);
                }
                ++orderCount;
            } catch (...) {
                cout << "A damaged order record was skipped while loading.\n";
            }
        }
        totalWasteValue = savedWaste;
        loadFeedback();
        cout << "Saved canteen data loaded. Orders restored: " << orderCount << '\n';
    }

    void saveFeedback() const {
        ofstream file("canteen_feedback.txt");
        if (!file) return;
        for (const auto& f : feedbackRecords)
            file << f.getToken() << '|' << f.getRating() << '|' << f.getComment() << '\n';
    }

    void loadFeedback() {
        ifstream file("canteen_feedback.txt");
        if (!file) return;
        string line;
        while (getline(file, line)) {
            stringstream row(line);
            string token, rating, comment;
            getline(row, token, '|'); getline(row, rating, '|'); getline(row, comment);
            try { feedbackRecords.emplace_back(stoi(token), stoi(rating), comment); }
            catch (...) { /* Ignore malformed feedback rows. */ }
        }
    }

    // ---------------------- Staff Login -----------------------
    void staffLogin() {
        if (staffAuthenticated) {
            cout << "Staff session is already active.\n";
            return;
        }
        string username = readLine("Staff username: ");
        string password = readLine("Staff PIN (demo PIN: 2468): ");
        if (username == "staff" && password == "2468") {
            staffAuthenticated = true;
            cout << "Staff login successful.\n";
            logActivity("Staff login successful");
        } else {
            cout << "Invalid staff credentials.\n";
            logActivity("Failed staff login attempt");
        }
    }

    void staffLogout() {
        staffAuthenticated = false;
        cout << "Staff session closed.\n";
    }

    // --------------------- Menu Management --------------------
    void editFoodItem() {
        if (!requireStaffAccess()) return;
        int id = readInt("Enter food ID to edit: ", 1);
        int index = findFoodBinary(id);
        if (index == -1) { cout << "Food item not found.\n"; return; }
        cout << "Current item: "; menu[index].display();
        string newName = readLine("New name (blank keeps current): ");
        string newCategory = readLine("New category (blank keeps current): ");
        string priceText = readLine("New price (blank keeps current): ");
        string name = newName.empty() ? menu[index].getName() : newName;
        string category = newCategory.empty() ? menu[index].getCategory() : newCategory;
        double price = menu[index].getPrice();
        if (!priceText.empty()) {
            try { price = stod(priceText); }
            catch (...) { cout << "Invalid price; edit cancelled.\n"; return; }
            if (price <= 0) { cout << "Price must be positive.\n"; return; }
        }
        menu[index] = FoodItem(id, name, category, price, menu[index].getQuantity());
        cout << "Menu item updated.\n";
        logActivity("Menu item edited: " + to_string(id));
        saveData();
    }

    void showLowStockItems() const {
        cout << "\n============= LOW STOCK ALERT =============\n";
        bool found = false;
        for (const auto& item : menu) {
            if (item.getQuantity() <= lowStockThreshold) {
                item.display();
                found = true;
            }
        }
        if (!found) cout << "No items are below the alert threshold.\n";
        cout << "Current threshold: " << lowStockThreshold << " units.\n";
    }

    void changeLowStockThreshold() {
        if (!requireStaffAccess()) return;
        lowStockThreshold = readInt("Set low-stock threshold: ", 0, 10000);
        cout << "Low-stock threshold updated.\n";
        logActivity("Low stock threshold changed to " + to_string(lowStockThreshold));
    }

    // --------------------- Order Management -------------------
    void showOrdersByStatus() const {
        cout << "1.Pending  2.Preparing  3.Ready  4.Collected  5.Cancelled\n";
        int choice = readInt("Choose status: ", 1, 5);
        OrderStatus wanted = static_cast<OrderStatus>(choice - 1);
        bool found = false;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].getStatus() == wanted) {
                orders[i].display();
                found = true;
            }
        }
        if (!found) cout << "No orders found in this status.\n";
    }

    void sortOrdersByBill() const {
        if (orderCount == 0) { cout << "No orders to sort.\n"; return; }
        vector<Order> sorted;
        for (int i = 0; i < orderCount; ++i) sorted.push_back(orders[i]);
        sort(sorted.begin(), sorted.end(), [](const Order& a, const Order& b) {
            if (a.getBill() == b.getBill()) return a.getToken() < b.getToken();
            return a.getBill() > b.getBill();
        });
        cout << "\nOrders sorted by bill (high to low):\n";
        for (const auto& order : sorted) order.display();
    }

    void showRecentOrders() const {
        int count = readInt("How many recent orders to display? ", 1, MAX_ORDERS);
        int start = max(0, orderCount - count);
        cout << "\nRecent orders (newest first):\n";
        for (int i = orderCount - 1; i >= start; --i) orders[i].display();
    }

    void estimateWaitTime() const {
        int pending = 0;
        int preparing = 0;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].getStatus() == OrderStatus::Pending) ++pending;
            if (orders[i].getStatus() == OrderStatus::Preparing) ++preparing;
        }
        const int minutesPerOrder = 5; // Demo assumption, not a live prediction.
        cout << "Estimated wait: " << (pending * minutesPerOrder + (preparing ? 3 : 0))
             << " minutes.\n";
        cout << "Estimate assumes one active preparation counter and 5 minutes/order.\n";
    }

    // --------------------- Customer Feedback ------------------
    void submitFeedback() {
        int token = readInt("Enter collected order token: ", 1);
        int index = findOrderLinear(token);
        if (index == -1 || orders[index].getStatus() != OrderStatus::Collected) {
            cout << "Feedback is accepted only for a collected order.\n";
            return;
        }
        for (const auto& f : feedbackRecords) {
            if (f.getToken() == token) {
                cout << "Feedback for this token has already been submitted.\n";
                return;
            }
        }
        int rating = readInt("Rate your experience (1 to 5): ", 1, 5);
        string comment = readLine("Short comment: ");
        feedbackRecords.emplace_back(token, rating, comment);
        cout << "Thank you for your feedback!\n";
        saveFeedback();
        logActivity("Feedback received for token " + to_string(token));
    }

    void showFeedbackReport() const {
        if (!requireStaffAccess()) return;
        if (feedbackRecords.empty()) { cout << "No feedback received yet.\n"; return; }
        double total = 0;
        cout << "\n=============== FEEDBACK REPORT ===============\n";
        for (const auto& feedback : feedbackRecords) {
            feedback.display();
            total += feedback.getRating();
        }
        cout << "Average rating: " << fixed << setprecision(2)
             << total / feedbackRecords.size() << "/5\n";
    }

    // --------------------- Sales Analytics --------------------
    void salesByFoodItem() const {
        cout << "\n============= SALES BY FOOD ITEM =============\n";
        double grandTotal = 0.0;
        for (const auto& item : menu) {
            int sold = 0;
            double amount = 0.0;
            for (int i = 0; i < orderCount; ++i) {
                if (orders[i].getFoodId() == item.getId() &&
                    orders[i].getStatus() == OrderStatus::Collected) {
                    sold += orders[i].getQuantity();
                    amount += orders[i].getBill();
                }
            }
            cout << left << setw(18) << item.getName()
                 << "Sold: " << setw(5) << sold
                 << "Sales: Rs. " << fixed << setprecision(2) << amount << '\n';
            grandTotal += amount;
        }
        cout << "Total collected sales: Rs. " << grandTotal << '\n';
    }

    void paymentMethodReport() const {
        int cash = 0, upi = 0, card = 0;
        double cashTotal = 0, upiTotal = 0, cardTotal = 0;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].getStatus() != OrderStatus::Collected) continue;
            string method = toLower(orders[i].getPaymentMethod());
            if (method.find("cash") != string::npos) { ++cash; cashTotal += orders[i].getBill(); }
            else if (method.find("upi") != string::npos) { ++upi; upiTotal += orders[i].getBill(); }
            else { ++card; cardTotal += orders[i].getBill(); }
        }
        cout << "\n=========== PAYMENT METHOD REPORT ===========\n";
        cout << "Cash: " << cash << " orders, Rs. " << cashTotal << '\n';
        cout << "UPI: " << upi << " orders, Rs. " << upiTotal << '\n';
        cout << "Card: " << card << " orders, Rs. " << cardTotal << '\n';
    }

    // ---------------------- CSV Export -------------------------
    void exportOrdersCSV() const {
        if (!requireStaffAccess()) return;
        ofstream file("canteen_orders_export.csv");
        if (!file) { cout << "Could not create CSV file.\n"; return; }
        file << "Token,Food ID,Food Name,Quantity,Bill,Payment,Priority,Status\n";
        for (int i = 0; i < orderCount; ++i) {
            const Order& o = orders[i];
            file << o.getToken() << ',' << o.getFoodId() << ",\"" << o.getFoodName()
                 << "\"," << o.getQuantity() << ',' << fixed << setprecision(2)
                 << o.getBill() << ',' << o.getPaymentMethod() << ','
                 << (o.isPriority() ? "Yes" : "No") << ','
                 << statusToString(o.getStatus()) << '\n';
        }
        cout << "Orders exported to canteen_orders_export.csv\n";
        logActivity("Order CSV export created");
    }

    void showActivityLog() const {
        if (!requireStaffAccess()) return;
        ifstream file("canteen_activity.log");
        if (!file) { cout << "No activity log available yet.\n"; return; }
        cout << "\n================ ACTIVITY LOG ================\n";
        string line;
        while (getline(file, line)) cout << line << '\n';
    }

    void showSystemHelp() const {
        cout << "\n================ SYSTEM HELP =================\n";
        cout << "1. Place an order and keep the generated token.\n";
        cout << "2. Normal orders follow FIFO using a circular queue.\n";
        cout << "3. Priority orders are selected before normal orders.\n";
        cout << "4. Staff can process, update, cancel and restore orders.\n";
        cout << "5. Cancelled orders return their quantity to inventory.\n";
        cout << "6. SAVE10 is the default demo coupon; staff can enable/disable it.\n";
        cout << "7. Data is stored in local text files in the launch directory.\n";
        cout << "8. UPI and card payments are simulations, not real transactions.\n";
        cout << "9. Demo staff login: username staff, PIN 2468.\n";
    }

    // ---------------- Advanced Menu Queries -----------------
    void searchByCategory() const {
        string category = toLower(readLine("Enter category name: "));
        bool found = false;
        for (const auto& item : menu) {
            if (toLower(item.getCategory()) == category) {
                item.display();
                found = true;
            }
        }
        if (!found) cout << "No items found in this category.\\n";
    }

    void showItemsWithinPriceRange() const {
        double minimum = 0.0, maximum = 0.0;
        while (true) {
            string lowText = readLine("Minimum price: ");
            string highText = readLine("Maximum price: ");
            try { minimum = stod(lowText); maximum = stod(highText); }
            catch (...) { cout << "Enter valid numeric prices.\\n"; continue; }
            if (minimum < 0 || maximum < minimum) {
                cout << "Invalid price range.\\n";
                continue;
            }
            break;
        }
        bool found = false;
        for (const auto& item : menu) {
            if (item.getPrice() >= minimum && item.getPrice() <= maximum) {
                item.display();
                found = true;
            }
        }
        if (!found) cout << "No menu item falls in this price range.\\n";
    }

    void displayMenuSortedByName() const {
        vector<FoodItem> sorted;
        for (const auto& item : menu) sorted.push_back(item);
        sort(sorted.begin(), sorted.end(), [](const FoodItem& a, const FoodItem& b) {
            return toLower(a.getName()) < toLower(b.getName());
        });
        cout << "\\nMenu sorted alphabetically by name:\\n";
        for (const auto& item : sorted) item.display();
    }

    // ---------------- Inventory Analytics -------------------
    void inventoryStockSummary() const {
        int totalUnits = 0;
        int distinctItems = 0;
        int outOfStock = 0;
        int lowStock = 0;
        for (const auto& item : menu) {
            totalUnits += item.getQuantity();
            if (item.getQuantity() > 0) ++distinctItems;
            else ++outOfStock;
            if (item.getQuantity() <= lowStockThreshold) ++lowStock;
        }
        cout << "\\n============= STOCK SUMMARY =============\\n";
        cout << "Different menu items: " << MENU_SIZE << '\n';
        cout << "Items currently available: " << distinctItems << '\n';
        cout << "Total units in stock: " << totalUnits << '\n';
        cout << "Out-of-stock item count: " << outOfStock << '\n';
        cout << "Low-stock item count: " << lowStock << '\n';
    }

    void showMostPopularFood() const {
        int bestIndex = -1;
        int bestQuantity = 0;
        for (int m = 0; m < MENU_SIZE; ++m) {
            int sold = 0;
            for (int i = 0; i < orderCount; ++i) {
                if (orders[i].getFoodId() == menu[m].getId() &&
                    orders[i].getStatus() != OrderStatus::Cancelled)
                    sold += orders[i].getQuantity();
            }
            if (sold > bestQuantity) {
                bestQuantity = sold;
                bestIndex = m;
            }
        }
        cout << "\\n============= POPULAR FOOD ITEM =============\\n";
        if (bestIndex == -1 || bestQuantity == 0) {
            cout << "No valid sales have been recorded yet.\\n";
            return;
        }
        cout << "Most ordered item: " << menu[bestIndex].getName() << '\n';
        cout << "Units ordered (excluding cancelled orders): " << bestQuantity << '\n';
    }

    void showOrderStatistics() const {
        int priorityCount = 0, normalCount = 0;
        int totalUnits = 0;
        double averageBill = 0.0;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].isPriority()) ++priorityCount;
            else ++normalCount;
            totalUnits += orders[i].getQuantity();
            averageBill += orders[i].getBill();
        }
        if (orderCount > 0) averageBill /= orderCount;
        cout << "\\n============= ORDER STATISTICS =============\\n";
        cout << "Normal orders: " << normalCount << '\n';
        cout << "Priority orders: " << priorityCount << '\n';
        cout << "Total food units ordered: " << totalUnits << '\n';
        cout << "Average bill value: Rs. " << fixed << setprecision(2) << averageBill << '\n';
        cout << "Order records stored: " << orderCount << '\n';
    }

    void exportInventoryCSV() const {
        if (!requireStaffAccess()) return;
        ofstream file("canteen_inventory_export.csv");
        if (!file) { cout << "Could not create inventory CSV file.\\n"; return; }
        file << "Food ID,Food Name,Category,Price,Available Stock,Stock Value\\n";
        for (const auto& item : menu) {
            file << item.getId() << ",\\\"" << item.getName() << "\\\",\\\""
                 << item.getCategory() << "\\\"," << fixed << setprecision(2)
                 << item.getPrice() << ',' << item.getQuantity() << ','
                 << item.getPrice() * item.getQuantity() << '\n';
        }
        cout << "Inventory exported to canteen_inventory_export.csv\\n";
        logActivity("Inventory CSV export created");
    }

    void printTokenSlip() const {
        int token = readInt("Enter token to print: ", 1);
        auto it = tokenIndex.find(token);
        if (it == tokenIndex.end()) { cout << "Token not found.\\n"; return; }
        const Order& order = orders[it->second];
        cout << "\\n========================================\\n";
        cout << "             COLLEGE CANTEEN\\n";
        cout << "              ORDER SLIP\\n";
        cout << "========================================\\n";
        cout << "Token Number : " << order.getToken() << '\n';
        cout << "Food Item    : " << order.getFoodName() << '\n';
        cout << "Quantity     : " << order.getQuantity() << '\n';
        cout << "Amount       : Rs. " << fixed << setprecision(2) << order.getBill() << '\n';
        cout << "Payment      : " << order.getPaymentMethod() << '\n';
        cout << "Order Type   : " << (order.isPriority() ? "Priority" : "Normal") << '\n';
        cout << "Status       : " << statusToString(order.getStatus()) << '\n';
        cout << "========================================\\n";
        cout << "Please keep this token for collection.\\n";
    }

    void showRevenueBreakdown() const {
        double gross = 0.0, cancelledValue = 0.0, collected = 0.0;
        for (int i = 0; i < orderCount; ++i) {
            gross += orders[i].getBill();
            if (orders[i].getStatus() == OrderStatus::Cancelled)
                cancelledValue += orders[i].getBill();
            if (orders[i].getStatus() == OrderStatus::Collected)
                collected += orders[i].getBill();
        }
        cout << "\\n============= REVENUE BREAKDOWN =============\\n";
        cout << "Total value of all created orders: Rs. " << fixed << setprecision(2) << gross << '\n';
        cout << "Cancelled order value: Rs. " << cancelledValue << '\n';
        cout << "Collected revenue: Rs. " << collected << '\n';
        cout << "Pending / in-progress value: Rs. " << gross - cancelledValue - collected << '\n';
        cout << "Note: this is a project estimate, not an accounting ledger.\\n";
    }

    // ---------------- Advanced DSA Lab: BST, Binary Tree, Graph -------------------
    // These structures are built from current menu/order workflow data at runtime.
    void showAdvancedDSALab() const {
        struct BSTNode {
            FoodItem item;
            unique_ptr<BSTNode> left;
            unique_ptr<BSTNode> right;
            explicit BSTNode(const FoodItem& value) : item(value) {}
        };

        auto insertBST = [&](auto&& self, unique_ptr<BSTNode>& node,
                             const FoodItem& item) -> void {
            if (!node) { node = make_unique<BSTNode>(item); return; }
            if (item.getId() < node->item.getId()) self(self, node->left, item);
            else if (item.getId() > node->item.getId()) self(self, node->right, item);
        };
        unique_ptr<BSTNode> root;
        // Median-first insertion keeps this demonstration tree balanced for the 8 starter IDs.
        const int insertionOrder[] = {4, 2, 6, 1, 3, 5, 7, 8};
        for (int id : insertionOrder) {
            int index = findFoodLinear(id);
            if (index >= 0) insertBST(insertBST, root, menu[index]);
        }

        auto printBST = [&](auto&& self, const BSTNode* node) -> void {
            if (!node) return;
            self(self, node->left.get());
            cout << "  ID " << node->item.getId() << " - " << node->item.getName() << '\n';
            self(self, node->right.get());
        };

        // Directed graph of allowed order-status transitions.
        // 0 Pending, 1 Preparing, 2 Ready, 3 Collected, 4 Cancelled.
        const vector<string> stateNames = {"Pending", "Preparing", "Ready", "Collected", "Cancelled"};
        vector<vector<int>> graph(5);
        graph[0] = {1, 4}; // Pending -> Preparing / Cancelled
        graph[1] = {2, 4}; // Preparing -> Ready / Cancelled
        graph[2] = {3, 4}; // Ready -> Collected / Cancelled

        auto runBFS = [&](int start) {
            vector<bool> visited(graph.size(), false);
            queue<int> q;
            q.push(start);
            visited[start] = true;
            cout << "BFS traversal: ";
            bool firstNode = true;
            while (!q.empty()) {
                int u = q.front(); q.pop();
                if (!firstNode) cout << " -> ";
                cout << stateNames[u];
                firstNode = false;
                for (int v : graph[u]) {
                    if (!visited[v]) { visited[v] = true; q.push(v); }
                }
            }
            cout << "\n";
        };
        auto runDFS = [&](int start) {
            vector<bool> visited(graph.size(), false);
            function<void(int)> dfs = [&](int u) {
                visited[u] = true;
                cout << stateNames[u] << " ";
                for (int v : graph[u]) if (!visited[v]) dfs(v);
            };
            cout << "DFS traversal: ";
            dfs(start);
            cout << "\n";
        };

        bool inLab = true;
        while (inLab) {
            cout << "\n\033[1;35m========== ADVANCED DSA LAB ==========\033[0m\n";
            cout << "\033[1;36m1. BST food search (by ID)\033[0m\n";
            cout << "\033[1;32m2. BST inorder traversal (sorted food IDs)\033[0m\n";
            cout << "\033[1;33m3. Binary tree - food category hierarchy\033[0m\n";
            cout << "\033[1;34m4. Graph BFS - order workflow\033[0m\n";
            cout << "\033[1;31m5. Graph DFS - order workflow\033[0m\n";
            cout << "\033[1;37m6. Check valid order status transition\033[0m\n";
            cout << "0. Back to System & Data\n";
            int choice = readInt("Choose DSA demo: ", 0, 6);
            if (choice == 0) { inLab = false; continue; }

            if (choice == 1) {
                int id = readInt("Enter Food ID to search in BST: ", 1);
                const BSTNode* current = root.get();
                while (current) {
                    cout << "Visit node ID " << current->item.getId() << "\n";
                    if (id == current->item.getId()) break;
                    current = (id < current->item.getId()) ? current->left.get() : current->right.get();
                }
                if (current) cout << "\033[1;32mBST FOUND: " << current->item.getName()
                                 << " | Rs. " << current->item.getPrice() << "\033[0m\n";
                else cout << "Food ID not found in BST.\n";
            } else if (choice == 2) {
                cout << "BST inorder (ascending Food ID):\n";
                printBST(printBST, root.get());
                cout << "Inorder traversal of a BST produces sorted keys.\n";
            } else if (choice == 3) {
                struct CategoryNode {
                    string label;
                    unique_ptr<CategoryNode> left, right;
                    explicit CategoryNode(string value) : label(std::move(value)) {}
                };
                CategoryNode tree("FOOD MENU");
                tree.left = make_unique<CategoryNode>("Snacks");
                tree.right = make_unique<CategoryNode>("Beverages");
                tree.left->left = make_unique<CategoryNode>("Samosa / Sandwich");
                tree.left->right = make_unique<CategoryNode>("Fast Food");
                tree.right->left = make_unique<CategoryNode>("Tea / Cold Coffee");
                tree.right->right = make_unique<CategoryNode>("Juice / Lemonade");
                cout << "\n             FOOD MENU\n"
                     << "             /      \\\n"
                     << "         Snacks    Beverages\n"
                     << "         /   \\      /    \\\n"
                     << " Samosa/Sandwich FastFood Tea/Coffee Juice\n";
                cout << "Binary tree demo: each node has at most two children.\n";
            } else if (choice == 4) {
                cout << "Workflow graph edges:\n"
                     << "Pending -> Preparing, Cancelled\n"
                     << "Preparing -> Ready, Cancelled\n"
                     << "Ready -> Collected, Cancelled\n";
                runBFS(0);
            } else if (choice == 5) {
                cout << "Starting DFS from Pending...\n";
                runDFS(0);
            } else if (choice == 6) {
                cout << "Status IDs: 0 Pending, 1 Preparing, 2 Ready, 3 Collected, 4 Cancelled\n";
                int from = readInt("Current status ID: ", 0, 4);
                int to = readInt("Next status ID: ", 0, 4);
                bool allowed = find(graph[from].begin(), graph[from].end(), to) != graph[from].end();
                cout << (allowed ? "\033[1;32mVALID transition\033[0m\n"
                                 : "\033[1;31mINVALID transition\033[0m\n");
            }
        }
    }

    // ---------------- Project Diagnostics -------------------
    void showProjectDiagnostics() const {
        cout << "\n============= PROJECT DIAGNOSTICS =============\n";
        cout << "C++ standard target : C++17\n";
        cout << "Menu storage        : Fixed-size array\n";
        cout << "Normal order flow   : Circular Queue (FIFO)\n";
        cout << "Priority order flow : Priority Queue / Heap\n";
        cout << "Undo operation      : Stack (LIFO)\n";
        cout << "Token lookup        : Unordered Map (average O(1))\n";
        cout << "Menu search         : Linear Search and Binary Search\n";
        cout << "Menu sorting        : Bubble Sort and STL sort\n";
        cout << "Food ID structure   : Binary Search Tree (BST) demo\n";
        cout << "Category structure  : Binary Tree demo\n";
        cout << "Order workflow      : Directed Graph (adjacency list)\n";
        cout << "Graph traversal     : BFS (queue) and DFS (recursion)\n";
        cout << "Order storage       : Array\n";
        cout << "Persistent files    : canteen_data.txt, canteen_feedback.txt\n";
        cout << "Activity file       : canteen_activity.log\n";
        cout << "\nComplexity overview (typical):\n";
        cout << "- Linear search: O(n)\n";
        cout << "- Binary search: O(log n), if IDs are sorted\n";
        cout << "- Circular queue enqueue/dequeue: O(1)\n";
        cout << "- Priority queue insert/remove: O(log n)\n";
        cout << "- Hash map token lookup: average O(1)\n";
        cout << "- Bubble sort: O(n^2)\n";
        cout << "- BST search: average O(log n), worst O(n)\n";
        cout << "- Graph BFS/DFS: O(V + E)\n";
    }

    void backupDataFiles() const {
        if (!requireStaffAccess()) return;
        const string files[] = {"canteen_data.txt", "canteen_feedback.txt", "canteen_activity.log"};
        const string backups[] = {"canteen_data_backup.txt", "canteen_feedback_backup.txt", "canteen_activity_backup.log"};
        int copied = 0;
        for (int i = 0; i < 3; ++i) {
            ifstream source(files[i], ios::binary);
            if (!source) continue;
            ofstream destination(backups[i], ios::binary);
            if (!destination) continue;
            destination << source.rdbuf();
            if (destination.good()) ++copied;
        }
        cout << copied << " data file(s) copied to backup files.\\n";
        logActivity("Local data backup attempted; files copied: " + to_string(copied));
    }

    void showDataFileNotice() const {
        cout << "\\n============= DATA STORAGE NOTICE =============\\n";
        cout << "This project saves data in ordinary text files.\\n";
        cout << "Keep the data files in the same folder as the executable.\\n";
        cout << "Do not edit the files while the application is running.\\n";
        cout << "The demo PIN is hard-coded and is not secure authentication.\\n";
        cout << "Payment methods are simulated; no bank or UPI network is contacted.\\n";
        cout << "The waiting-time figure is an estimate based on a fixed assumption.\\n";
    }

    void configureCoupon() {
        if (!requireStaffAccess()) return;
        cout << "Current coupon: " << activeCoupon.getCode()
             << " (" << activeCoupon.getDiscountPercent() << "%).\n";
        cout << "1. Enable SAVE10\n2. Disable SAVE10\n";
        int choice = readInt("Choose: ", 1, 2);
        activeCoupon.setActive(choice == 1);
        cout << "Coupon status updated.\n";
        logActivity(choice == 1 ? "Coupon enabled" : "Coupon disabled");
    }

    void showCategorySummary() const {
        cout << "\n============= CATEGORY SUMMARY =============\n";
        vector<string> categories;
        for (const auto& item : menu) {
            if (find(categories.begin(), categories.end(), item.getCategory()) == categories.end())
                categories.push_back(item.getCategory());
        }
        for (const string& category : categories) {
            int count = 0;
            for (const auto& item : menu) if (item.getCategory() == category) ++count;
            cout << category << ": " << count << " menu item(s)\n";
        }
    }

    void showInventoryValuation() const {
        double total = 0.0;
        cout << "\n=========== INVENTORY VALUATION ===========\n";
        for (const auto& item : menu) {
            double value = item.getPrice() * item.getQuantity();
            total += value;
            cout << left << setw(18) << item.getName()
                 << "Qty: " << setw(5) << item.getQuantity()
                 << "Value: Rs. " << fixed << setprecision(2) << value << '\n';
        }
        cout << "Total estimated inventory value: Rs. " << total << '\n';
    }

    void shutdownAndSave() {
        saveData();
        cout << "Data saved successfully.\n";
    }
};


// --------------------- DESIGNED TERMINAL UI -------------------
struct MenuOption {
    int id;
    string label;
};

// --------------------- COLOURFUL TERMINAL UI -----------------
namespace UI {
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string CYAN = "\033[1;36m";
    const string YELLOW = "\033[1;33m";
    const string GREEN = "\033[1;32m";
    const string BLUE = "\033[1;34m";
    const string MAGENTA = "\033[1;35m";
    const string RED = "\033[1;31m";
    const string WHITE = "\033[1;37m";
    const string DIM = "\033[0;90m";
    const string colors[6] = {CYAN, GREEN, YELLOW, BLUE, MAGENTA, RED};
}

void printLine(char ch = '=', int width = 95) {
    cout << string(width, ch) << '\n';
}

void printHeader(const string& title, const string& subtitle = "") {
    cout << "\033[2J\033[H";
    printLine('=');
    cout << UI::CYAN << UI::BOLD
         << "                    * * *  SMART CANTEEN  * * *" << UI::RESET << '\n';
    cout << UI::YELLOW << UI::BOLD
         << "                 QUEUE  |  TOKEN  |  ORDER MANAGEMENT" << UI::RESET << '\n';
    printLine('=');
    cout << UI::WHITE << UI::BOLD << "  " << title << UI::RESET << '\n';
    if (!subtitle.empty())
        cout << UI::DIM << "  " << subtitle << UI::RESET << '\n';
    printLine('-');
}

// True 3-column x 2-row dashboard with aligned vertical borders.
void printCards(const vector<pair<string, string>>& cards) {
    const int cell = 29;
    const int rows = static_cast<int>((cards.size() + 2) / 3);
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < 3; ++c) cout << "+" << string(cell, '-');
        cout << "+\n";
        for (int line = 0; line < 2; ++line) {
            for (int c = 0; c < 3; ++c) {
                int idx = r * 3 + c;
                cout << "| ";
                if (idx < static_cast<int>(cards.size())) {
                    string value = line == 0
                        ? (cards[idx].first + "  " + cards[idx].second)
                        : "  OPEN SECTION  >";
                    if (value.size() > static_cast<size_t>(cell - 2)) value.resize(cell - 2);
                    cout << UI::colors[idx % 6] << UI::BOLD
                         << left << setw(cell - 2) << value << UI::RESET;
                } else cout << left << setw(cell - 2) << " ";
            }
            cout << "|\n";
        }
    }
    for (int c = 0; c < 3; ++c) cout << "+" << string(cell, '-');
    cout << "+\n";
}

// Submenu actions are displayed left-to-right, then top-to-bottom.
void printOptionsGrid(const vector<MenuOption>& options) {
    const int cell = 29;
    cout << UI::DIM << "  AVAILABLE ACTIONS  |  Read left to right, row by row" << UI::RESET << '\n';
    for (size_t i = 0; i < options.size(); i += 3) {
        for (int c = 0; c < 3; ++c) cout << "+" << string(cell, '-');
        cout << "+\n";
        for (int c = 0; c < 3; ++c) {
            size_t idx = i + c;
            cout << "| ";
            if (idx < options.size()) {
                string number = "[" + to_string(options[idx].id) + "]";
                string label = options[idx].label;
                int available = cell - 2;
                if (static_cast<int>(number.size() + label.size() + 1) > available)
                    label.resize(max(0, available - static_cast<int>(number.size()) - 1));
                cout << UI::YELLOW << UI::BOLD << left << setw(number.size())
                     << number << UI::RESET << " "
                     << left << setw(available - number.size() - 1) << label;
            } else cout << left << setw(cell - 2) << " ";
        }
        cout << "|\n";
    }
    for (int c = 0; c < 3; ++c) cout << "+" << string(cell, '-');
    cout << "+\n";
}

int runAction(CanteenSystem& canteen, Staff& staff, int choice) {
    switch (choice) {
        case 1: canteen.showMenu(); break;
        case 2: canteen.searchFood(); break;
        case 3: canteen.searchFoodByName(); break;
        case 4: canteen.displayMenuSortedByPrice(); break;
        case 5: canteen.placeOrder(); break;
        case 6: staff.showRole(); canteen.processNextOrder(); break;
        case 7: canteen.updateOrderStatus(); break;
        case 8: canteen.searchOrderByToken(); break;
        case 9: canteen.showAllOrders(); break;
        case 10: canteen.showQueueSummary(); break;
        case 11: canteen.cancelOrder(); break;
        case 12: canteen.undoCancellation(); break;
        case 13: canteen.showInventory(); break;
        case 14: canteen.restockItem(); break;
        case 15: canteen.recordWaste(); break;
        case 16: staff.showRole(); canteen.showStaffDashboard(); break;
        case 17: canteen.showDailyReport(); break;
        case 18: canteen.staffLogin(); break;
        case 19: canteen.staffLogout(); break;
        case 20: canteen.editFoodItem(); break;
        case 21: canteen.showLowStockItems(); break;
        case 22: canteen.changeLowStockThreshold(); break;
        case 23: canteen.showOrdersByStatus(); break;
        case 24: canteen.sortOrdersByBill(); break;
        case 25: canteen.showRecentOrders(); break;
        case 26: canteen.estimateWaitTime(); break;
        case 27: canteen.submitFeedback(); break;
        case 28: canteen.showFeedbackReport(); break;
        case 29: canteen.salesByFoodItem(); break;
        case 30: canteen.paymentMethodReport(); break;
        case 31: canteen.exportOrdersCSV(); break;
        case 32: canteen.showActivityLog(); break;
        case 33: canteen.configureCoupon(); break;
        case 34: canteen.showCategorySummary(); break;
        case 35: canteen.showInventoryValuation(); break;
        case 36: canteen.showSystemHelp(); break;
        case 37: canteen.searchByCategory(); break;
        case 38: canteen.showItemsWithinPriceRange(); break;
        case 39: canteen.displayMenuSortedByName(); break;
        case 40: canteen.inventoryStockSummary(); break;
        case 41: canteen.showMostPopularFood(); break;
        case 42: canteen.showOrderStatistics(); break;
        case 43: canteen.exportInventoryCSV(); break;
        case 44: canteen.printTokenSlip(); break;
        case 45: canteen.showRevenueBreakdown(); break;
        case 46: canteen.showProjectDiagnostics(); break;
        case 47: canteen.backupDataFiles(); break;
        case 48: canteen.showDataFileNotice(); break;
        case 49: canteen.showAdvancedDSALab(); break;
        default: cout << "Invalid option.\n"; return 0;
    }
    return 1;
}

int main() {
    CanteenSystem canteen;
    Staff staff(9001, "Canteen Staff");

    const vector<string> categories = {
        "Customer Services", "Order Management", "Billing & Reports",
        "Inventory Control", "Staff Dashboard", "System & Data"
    };

    const vector<vector<MenuOption>> menus = {
        {
            {1, "View food menu"}, {2, "Search food by ID"}, {3, "Search by name"},
            {4, "Sort menu by price"}, {5, "Place order / token"}, {9, "View saved orders"},
            {37, "Search by category"},
            {38, "Filter by price"}, {39, "Sort menu by name"}, {27, "Submit feedback"},
            {44, "Print token slip"}
        },
        {
            {6, "Process next order"}, {7, "Update order status"}, {8, "Find order by token"},
            {9, "View all orders"}, {10, "Queue summary"}, {11, "Cancel order"},
            {12, "Undo cancellation"}, {23, "Filter by status"}, {24, "Sort by bill"},
            {25, "Recent orders"}, {26, "Estimated wait time"}
        },
        {
            {17, "Daily report"}, {29, "Sales by food item"}, {30, "Payment report"},
            {31, "Export orders CSV"}, {33, "Configure SAVE10"}, {41, "Popular food item"},
            {42, "Order statistics"}, {45, "Revenue breakdown"}, {28, "Feedback report"}
        },
        {
            {13, "View inventory"}, {14, "Restock item"}, {15, "Record food waste"},
            {20, "Edit food item"}, {21, "Low-stock alerts"}, {22, "Stock threshold"},
            {34, "Category summary"}, {35, "Inventory valuation"}, {40, "Stock summary"},
            {43, "Export inventory CSV"}
        },
        {
            {18, "Staff login"}, {19, "Staff logout"}, {16, "Staff dashboard"},
            {6, "Process next order"}, {17, "Daily report"}, {21, "Low-stock alerts"}
        },
        {
            {36, "System help"}, {46, "DSA diagnostics"}, {47, "Backup data files"},
            {48, "Storage & security notice"}, {32, "Activity log"}, {49, "Advanced DSA Lab"}
        }
    };

    canteen.showWelcomeOverview();
    cout << "  Press ENTER to open the dashboard...";
    cin.get();

    bool running = true;
    while (running) {
        printHeader("MAIN DASHBOARD", "Choose a section • Options are arranged horizontally for quick scanning");
        cout << "\n";
        printCards({
            {"1", "CUSTOMER SERVICES"}, {"2", "ORDER MANAGEMENT"}, {"3", "BILLING & REPORTS"},
            {"4", "INVENTORY CONTROL"}, {"5", "STAFF DASHBOARD"}, {"6", "SYSTEM & DATA"}
        });
        printLine('-');
        cout << UI::GREEN << UI::BOLD << "  [0] SAVE & EXIT" << UI::RESET << '\n';
        printLine('-');

        int section = readInt(string(UI::CYAN) + UI::BOLD + "  Choose section [0-6] > " + UI::RESET, 0, 6);
        if (section == 0) {
            canteen.shutdownAndSave();
            cout << "\n\033[1;32mThank you! Your canteen data has been saved.\033[0m\n";
            running = false;
            continue;
        }

        bool inSection = true;
        while (inSection) {
            printHeader(categories[section - 1], "Select an action • Enter 0 to return to dashboard");
            printOptionsGrid(menus[section - 1]);
            printLine('-');
            cout << UI::MAGENTA << UI::BOLD << "  [0] BACK TO MAIN DASHBOARD" << UI::RESET << '\n';
            printLine('-');

            int action = readInt(string(UI::CYAN) + UI::BOLD + "  Choose action number [0 = Back] > " + UI::RESET, 0, 49);
            if (action == 0) {
                inSection = false;
                continue;
            }

            bool belongs = false;
            for (const auto& option : menus[section - 1]) {
                if (option.id == action) { belongs = true; break; }
            }
            if (!belongs) {
                cout << "\n  That option is not in this section. Please choose one shown above.\n";
            } else {
                cout << "\n";
                runAction(canteen, staff, action);
            }

            cout << "\n";
            printLine('-');
            cout << "  Press ENTER to continue...";
            cin.get();
        }
    }
    return 0;
}
