#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;


// Structure to store logged-in waiters and their orders
struct Waiter {
    string username;
    int tableNumber;
    vector<string> orderItems;  // Store the items for each waiter
};

// Function to register a new user
void registerUser() {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;

    if (users.find(username) != users.end()) {
        cout << "Username already exists. Try another one.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> password;

    // Save user to in-memory map
    users[username] = password;

    // Append the new user to the users.txt file
    ofstream userFile("users.txt", ios::app);  // Open file in append mode
    if (userFile.is_open()) {
        userFile << username << ":" << password << endl;  // Write the username and password to the file
        cout << "Registration successful! You can now log in.\n";
        userFile.close();  // Close the file after writing
    } else {
        cout << "Failed to save user data.\n";
    }
}

// Function for login verification
bool login(string& username) {
    string password, storedUsername, storedPassword;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    // Open users.txt to verify credentials
    ifstream userFile("users.txt");

    if (userFile.is_open()) {
        while (getline(userFile, storedUsername, ':')) {
            getline(userFile, storedPassword);  // Read the password

            // Check if the entered username and password match
            if (storedUsername == username && storedPassword == password) {
                cout << "Login successful for " << username << "!\n";
                userFile.close();
                return true;  // User found and logged in successfully
            }
        }
        userFile.close();
    }

    cout << "Invalid credentials. Try again.\n";
    return false;  // Invalid credentials
}

// Function to take orders from the customer
void takeOrderFromCustomer(Waiter& waiter) {
    string item;
    char choice;

    while (true) {
        cout << "What would you like to order? (Type 'done' to finish): ";
        cin.ignore(); // To clear input buffer
        getline(cin, item);

        if (item == "done") {
            break;
        }

        waiter.orderItems.push_back(item);  // Add the item to the order
        cout << "Would you like to add more items? (y/n): ";
        cin >> choice;

        if (choice == 'n' || choice == 'N') {
            break;
        }
    }

    // Save the orders to orders.txt
    ofstream orderFile("orders.txt", ios::app);  // Open file in append mode
    if (orderFile.is_open()) {
        orderFile << "Waiter: " << waiter.username << "\n";
        orderFile << "Table: " << waiter.tableNumber << "\n";
        orderFile << "Order Items:\n";
        for (const string& item : waiter.orderItems) {
            orderFile << "- " << item << "\n";
        }
        orderFile << "------------------------\n";
        orderFile.close();
        cout << "Order has been saved!\n";
    } else {
        cout << "Failed to save order data.\n";
    }
}

// Function to create child processes for order processing
void createChildProcesses(vector<Waiter> waiters) {
    for (Waiter& waiter : waiters) {
        cout << " Processing order for Table " << waiter.tableNumber << " (Waiter: " << waiter.username << ")...\n";

        // Setup process startup info
        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        // Command to execute child process (restaurant_child.exe)
        string command = "restaurant_child.exe " + to_string(waiter.tableNumber);

        // Convert command to LPSTR
        char cmd[MAX_PATH];
        strcpy_s(cmd, command.c_str());

        // Create a new process for each table order
        if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            cout << "Failed to create process for Table " << waiter.tableNumber << endl;
            continue;
        }

        // Wait for the process to finish (optional)
        WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

int main() {
    cout << " Welcome to the Restaurant System!\n";

    int choice;
    cout << "1. Register\n2. Login\nChoose an option: ";
    cin >> choice;

    if (choice == 1) {
        registerUser();
    }

    int numWaiters;
    cout << "Enter number of waiters logging in: ";
    cin >> numWaiters;

    vector<Waiter> waiters;
    for (int i = 0; i < numWaiters; i++) {
        string username;
        cout << "🔑 Waiter " << i + 1 << " login:\n";
        while (!login(username)) {
            cout << "Login failed. Try again.\n";
        }

        int tableNumber;
        cout << "Enter Table Number for Order (Waiter: " << username << "): ";
        cin >> tableNumber;

        Waiter newWaiter = {username, tableNumber};
        takeOrderFromCustomer(newWaiter);  // Take order for the waiter

        waiters.push_back(newWaiter);  // Save waiter and their orders
    }

    createChildProcesses(waiters);

    cout << " All orders have been processed!\n";
    return 0;
}
