#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

// Function to generate an 8-character barcode for an order
string generateBarcode(int tableNumber) {
    string barcode = "TBL" + to_string(tableNumber);
    while (barcode.length() < 8) {
        barcode += char('A' + rand() % 26);
    }
    return barcode;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "❌ Error: No table number provided.\n";
        return 1;
    }

    int tableNumber = atoi(argv[1]); // Convert argument to int
    string barcode = generateBarcode(tableNumber);
    
    cout << "✅ Order confirmed for Table " << tableNumber << " - Barcode: " << barcode << "\n";
    Sleep(3000); // Simulate processing time
    cout << "🔵 Order processing complete for Table " << tableNumber << ".\n";

    return 0;
}
