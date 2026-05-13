#pragma once
#ifndef ADMIN_H
#define ADMIN_H

#include "Person.h"

class Admin : public Person {
public:
    // Uses the base Person constructor directly
    Admin(int id = 0, const char* name = "", const char* password = "");

    // Overridden base functions
    void displayMenu() const override;
    const char* getRole() const override;
};

#endif