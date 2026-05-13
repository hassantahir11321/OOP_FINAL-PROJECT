#pragma once
#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"
#include <iostream>
#include <cstring>

class Doctor : public Person {
private:
    char* specialization;
    char* contact;
    double fee;

public:
    // Constructor with default arguments
    Doctor(int id = 0, const char* name = "", const char* specialization = "",
        const char* contact = "", const char* password = "", double fee = 0.0);

    // Copy Constructor & Assignment Operator
    Doctor(const Doctor& other);
    Doctor& operator=(const Doctor& other);

    // Destructor
    ~Doctor() override;

    // Getters
    const char* getSpecialization() const;
    const char* getContact() const;
    double getFee() const;

    // Overridden base functions
    void displayMenu() const override;
    const char* getRole() const override;

    // Required Operator Overloads
    bool operator==(const Doctor& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Doctor& doctor);
};

#endif