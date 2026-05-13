#include "Validator.h"
#include "Storage.h" 
#include "Admin.h"   
#include <iostream>

// --- ADMIN LOGIN VALIDATION ---
bool Validator::validateAdmin(const char* inputId, const char* inputPassword, Storage<Admin>& admins) {
    // Convert the input string ID to an integer to match Admin's int ID
    int idAsInt = 0;
    for (int i = 0; inputId[i] != '\0'; i++) {
        if (inputId[i] >= '0' && inputId[i] <= '9') {
            idAsInt = idAsInt * 10 + (inputId[i] - '0');
        }
    }

    for (int i = 0; i < admins.size(); i++) {
        Admin currentAdmin = admins.getAll()[i];

        if (idAsInt == currentAdmin.getId() &&
            Validator::stringEquals(inputPassword, currentAdmin.getPassword())) {
            return true;
        }
    }
    return false;
}

// --- CUSTOM STRING UTILITIES ---
bool Validator::stringEquals(const char* str1, const char* str2) {
    if (str1 == nullptr || str2 == nullptr) return false;

    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return false;
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0');
}

int Validator::stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// --- DUMMY DEFINITIONS FOR WARNINGS ---
// Add your actual logic for these later, but these will clear the errors now:
bool Validator::isValidDate(const char* date) { return true; }
bool Validator::isValidTimeSlot(const char* timeSlot) { return true; }
bool Validator::isValidContact(const char* contact) { return true; }
bool Validator::isValidPassword(const char* password) { return true; }
bool Validator::isPositiveFloat(double value) { return value > 0; }
bool Validator::isValidMenuChoice(int choice, int min, int max) {
    return (choice >= min && choice <= max);
}