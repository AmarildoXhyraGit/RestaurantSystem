#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "sqlite3.h"

void initializeDatabase();
void closeDatabase();
void registerUser();
bool login(std::string& username);
bool isTableOccupied(int tableNumber);
void saveOrder(const std::string& waiter, int tableNumber, const std::string& orderItems);

#endif
