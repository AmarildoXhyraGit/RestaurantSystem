#include <iostream>
#include <string>
#include <windows.h>
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Error: Table number not provided.\n";
        return 1;
    }

    int tableNumber = atoi(argv[1]);
    cout << "Processing order for Table " << tableNumber << "...\n";

    // Simulate order processing (can be expanded)
    Sleep(2000);  // Simulate processing time (2 seconds)
    
    cout << "Order confirmed for Table " << tableNumber << "!\n";
    return 0;
}
