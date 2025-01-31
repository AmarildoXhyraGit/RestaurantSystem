#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <ctime>

using namespace std;

// Predefined usernames and passwords
unordered_map<string, string> users = {
    {"waiter1", "pass123"},
    {"waiter2", "abc123"},
    {"waiter3", "xyz789"}
};

// Structure to store logged-in waiters and their orders
struct Waiter {
    string username;
    int tableNumber;
    vector<string> orderItems;
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

    users[username] = password;
    cout << "Registration successful! You can now log in.\n";
}

// Function for login verification
bool login(string& username) {
    string password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    if (users.find(username) != users.end() && users[username] == password) {
        cout << "Login successful for " << username << "!\n";
        return true;
    } else {
        cout << "Invalid credentials. Try again.\n";
        return false;
    }
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
}

// Function to check if the table number has already been assigned
bool isTableAssigned(int tableNumber, const vector<Waiter>& waiters) {
    for (const Waiter& waiter : waiters) {
        if (waiter.tableNumber == tableNumber) {
            return true; // Table number already assigned
        }
    }
    return false; // Table number is available
}

// Function to create child processes for order processing
void createChildProcesses(vector<Waiter>& waiters) {
    for (Waiter& waiter : waiters) {
        cout << "Processing order for Table " << waiter.tableNumber << " (Waiter: " << waiter.username << ")...\n";
        
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
    cout << "Welcome to the Restaurant System!\n";

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
        cout << "Waiter " << i + 1 << " login:\n";
        while (!login(username)) {
            cout << "Login failed. Try again.\n";
        }

        int tableNumber;
        // Ask for a table number and ensure it is not already taken
        while (true) {
            cout << "Enter Table Number for Order (Waiter: " << username << "): ";
            cin >> tableNumber;

            if (isTableAssigned(tableNumber, waiters)) {
                cout << "Table " << tableNumber << " is already assigned. Please choose a different table number.\n";
            } else {
                break; // Valid table number
            }
        }
        
        Waiter newWaiter = {username, tableNumber};
        takeOrderFromCustomer(newWaiter);  // Take order for the waiter
        
        waiters.push_back(newWaiter);  // Save waiter and their orders
    }

    createChildProcesses(waiters);

    cout << "All orders have been processed!\n";
    return 0;
}
