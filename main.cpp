#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <sqlite3.h>

using namespace std;

// Database connection
sqlite3* db;

// Function to open the database
bool openDatabase() {
    int rc = sqlite3_open("restaurant.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

// Function to create tables if they don't exist
void createTables() {
    const char* createUsersTable = "CREATE TABLE IF NOT EXISTS Users ("
                                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                   "username TEXT UNIQUE NOT NULL, "
                                   "password TEXT NOT NULL);";

    const char* createOrdersTable = "CREATE TABLE IF NOT EXISTS Orders ("
                                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "table_number INTEGER NOT NULL, "
                                    "barcode TEXT UNIQUE NOT NULL, "
                                    "status TEXT DEFAULT 'Pending', "
                                    "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP);";

    char* errMsg = 0;
    sqlite3_exec(db, createUsersTable, NULL, 0, &errMsg);
    sqlite3_exec(db, createOrdersTable, NULL, 0, &errMsg);
}

// Function to register a new user
void registerUser() {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;

    // Check if user already exists
    string sql = "SELECT COUNT(*) FROM Users WHERE username = '" + username + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count > 0) {
        cout << "Username already exists. Try another one.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> password;

    // Insert new user
    sql = "INSERT INTO Users (username, password) VALUES ('" + username + "', '" + password + "');";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        cerr << "Error registering user: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Registration successful! You can now log in.\n";
    }
}

// Function for login verification
bool login() {
    string username, password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    string sql = "SELECT COUNT(*) FROM Users WHERE username = '" + username + "' AND password = '" + password + "';";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count > 0) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Invalid credentials. Try again.\n";
        return false;
    }
}

// Function to generate an 8-character barcode for an order
string generateBarcode(int tableNumber) {
    string barcode = "TBL" + to_string(tableNumber);
    while (barcode.length() < 8) {
        barcode += char('A' + rand() % 26);
    }
    return barcode;
}

// Function to save an order in the database
void saveOrderToDB(int tableNumber, string barcode) {
    string sql = "INSERT INTO Orders (table_number, barcode) VALUES (" + to_string(tableNumber) + ", '" + barcode + "');";
    char* errMsg = 0;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        cerr << "Error saving order: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Order saved in database!\n";
    }
}

// Function to process orders (Child process)
DWORD WINAPI processOrder(LPVOID param) {
    int tableNumber = *(int*)param;
    string barcode = generateBarcode(tableNumber);
    saveOrderToDB(tableNumber, barcode);
    cout << "✅ Order confirmed for Table " << tableNumber << " - Barcode: " << barcode << "\n";
    Sleep(3000); // Simulate processing time
    cout << "🔵 Order processing complete for Table " << tableNumber << ".\n";
    return 0;
}

// Function for taking orders (Parent process)
void takeOrders(vector<int> tableNumbers) {
    vector<HANDLE> orderThreads;
    
    for (int table : tableNumbers) {
        cout << "⏳ Processing order for Table " << table << "...\n";
        HANDLE hThread = CreateThread(NULL, 0, processOrder, &table, 0, NULL);
        orderThreads.push_back(hThread);
    }
    
    // Wait for all threads to complete
    for (HANDLE thread : orderThreads) {
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(thread);
    }
}

// Function to display all past orders from the database
void displayOrders() {
    cout << "\n📜 All Orders:\n";
    string sql = "SELECT table_number, barcode, status, timestamp FROM Orders;";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int tableNumber = sqlite3_column_int(stmt, 0);
        string barcode = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

        cout << "Table " << tableNumber << " | Barcode: " << barcode << " | Status: " << status << " | Time: " << timestamp << "\n";
    }
    sqlite3_finalize(stmt);
}

int main() {
    srand(time(0));

    if (!openDatabase()) return 1;
    createTables();

    cout << "🏨 Welcome to the Restaurant System!\n";

    int choice;
    cout << "1. Register\n2. Login\nChoose an option: ";
    cin >> choice;

    if (choice == 1) {
        registerUser();
    }

    if (!login()) {
        return 1; // Exit if login fails
    }

    int numWaiters;
    cout << "Enter number of waiters logging in: ";
    cin >> numWaiters;
    
    vector<int> tableNumbers;
    for (int i = 0; i < numWaiters; i++) {
        int tableNumber;
        cout << "Enter Table Number for Order (Waiter " << i + 1 << "): ";
        cin >> tableNumber;
        tableNumbers.push_back(tableNumber);
    }
    
    takeOrders(tableNumbers);

    cout << "\n📌 Do you want to see all past orders? (1 = Yes, 0 = No): ";
    int showOrders;
    cin >> showOrders;
    if (showOrders == 1) {
        displayOrders();
    }

    sqlite3_close(db);
    cout << "All orders have been processed!\n";
    return 0;
}
