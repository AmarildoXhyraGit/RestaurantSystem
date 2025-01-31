#include <iostream>
#include "database.h"

using namespace std;

sqlite3* db;
char* errMsg = 0;

// Initialize the database and create tables
void initializeDatabase() {
    int rc = sqlite3_open("restaurant.db", &db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        exit(1);
    }

    const char* createUsersTable =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT UNIQUE NOT NULL,"
        "password TEXT NOT NULL);";

    const char* createOrdersTable =
        "CREATE TABLE IF NOT EXISTS orders ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "waiter TEXT NOT NULL,"
        "table_number INTEGER UNIQUE NOT NULL,"
        "order_items TEXT NOT NULL);";

    sqlite3_exec(db, createUsersTable, 0, 0, &errMsg);
    sqlite3_exec(db, createOrdersTable, 0, 0, &errMsg);
}

// Close the database
void closeDatabase() {
    sqlite3_close(db);
}

// Register a new user
void registerUser() {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;
    cout << "Enter new password: ";
    cin >> password;

    string sql = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + password + "');";
    if (sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg) == SQLITE_OK) {
        cout << "✅ Registration successful!\n";
    } else {
        cout << "❌ Error: " << sqlite3_errmsg(db) << endl;
    }
}

// Login function
bool login(string& username) {
    string password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    string sql = "SELECT * FROM users WHERE username = '" + username + "' AND password = '" + password + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return true;
        }
    }
    sqlite3_finalize(stmt);
    return false;
}

// Check if table is already occupied
bool isTableOccupied(int tableNumber) {
    string sql = "SELECT * FROM orders WHERE table_number = " + to_string(tableNumber) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            sqlite3_finalize(stmt);
            return true;
        }
    }
    sqlite3_finalize(stmt);
    return false;
}

// Save order in the database
void saveOrder(const string& waiter, int tableNumber, const string& orderItems) {
    string sql = "INSERT INTO orders (waiter, table_number, order_items) VALUES ('" + waiter + "', " + to_string(tableNumber) + ", '" + orderItems + "');";
    sqlite3_exec(db, sql.c_str(), 0, 0, &errMsg);
}
