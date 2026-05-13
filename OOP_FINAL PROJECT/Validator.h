#pragma once
#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "Storage.h"
#include "Admin.h"

class Validator {
public:
    // --- Custom String Utilities to replace banned functions ---
    static int stringLength(const char* str);
    static void stringCopy(char* dest, const char* src);
    static bool stringEquals(const char* str1, const char* str2);
    static bool stringEqualsIgnoreCase(const char* str1, const char* str2);

    // --- Required Assignment Validations ---
    static bool isValidDate(const char* date);       // Must be DD-MM-YYYY
    static bool isValidTimeSlot(const char* timeSlot); // Must be one of the 8 slots
    static bool isValidContact(const char* contact);   // Exactly 11 digits
    static bool isValidPassword(const char* password); // Minimum 6 characters
    static bool isPositiveFloat(double value);         // Must be > 0
    static bool isValidMenuChoice(int choice, int min, int max);

    // --- NEW: Admin Login Validation ---
    static bool validateAdmin(const char* inputId, const char* inputPassword, Storage<Admin>& admins);
};

#endif