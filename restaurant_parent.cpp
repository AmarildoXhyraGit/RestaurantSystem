#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// Predefined usernames and passwords
unordered_map<string, string> users = {
    {"waiter1", "pass123"},
    {"waiter2", "abc123"},
    {"waiter3", "xyz789"}
};

// Structure to store logged-in waiters and their tables
struct Waiter {
    string username;
    int tableNumber;
};

// Function to register a new user
void registerUser() {
    string username, password;
    cout << "Enter new username: ";
    cin >> username;

    if (users.find(username) != users.end()) {
        cout << "❌ Username already exists. Try another one.\n";
        return;
    }

    cout << "Enter new password: ";
    cin >> password;

    users[username] = password;
    cout << "✅ Registration successful! You can now log in.\n";
}

// Function for login verification
bool login(string& username) {
    string password;
    cout << "Enter Username: ";
    cin >> username;
    cout << "Enter Password: ";
    cin >> password;

    if (users.find(username) != users.end() && users[username] == password) {
        cout << "✅ Login successful for " << username << "!\n";
        return true;
    } else {
        cout << "❌ Invalid credentials. Try again.\n";
        return false;
    }
}

// Function to create child processes for order processing
void createChildProcesses(vector<Waiter> waiters) {
    for (Waiter waiter : waiters) {
        cout << "⏳ Processing order for Table " << waiter.tableNumber << " (Waiter: " << waiter.username << ")...\n";
        
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
            cout << "❌ Failed to create process for Table " << waiter.tableNumber << endl;
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
    cout << "🏨 Welcome to the Restaurant System!\n";

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
            cout << "❌ Login failed. Try again.\n";
        }

        int tableNumber;
        cout << "Enter Table Number for Order (Waiter: " << username << "): ";
        cin >> tableNumber;
        
        waiters.push_back({username, tableNumber});
    }

    createChildProcesses(waiters);

    cout << "✅ All orders have been processed!\n";
    return 0;
}
