#pragma once
#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"
#include <iostream>

class Patient : public Person {
private:
    int age;
    char gender;
    char* contact;
    double balance;

public:
    // Default arguments help when creating arrays/Storage later
    Patient(int id = 0, const char* name = "", int age = 0, char gender = ' ',
        const char* contact = "", const char* password = "", double balance = 0.0);

    // Copy Constructor & Assignment Operator (required for dynamic memory)
    Patient(const Patient& other);
    Patient& operator=(const Patient& other);

    // Destructor
    ~Patient() override;

    // Getters for Patient-specific data
    int getAge() const;
    char getGender() const;
    const char* getContact() const;
    double getBalance() const;

    // Overridden pure virtual functions from Person
    void displayMenu() const override;
    const char* getRole() const override;

    // --- Required Operator Overloads ---
    Patient& operator+=(double amount);
    Patient& operator-=(double amount);
    bool operator==(const Patient& other) const;

    // Friend function to overload the << operator for clean console output
    friend std::ostream& operator<<(std::ostream& os, const Patient& patient);
};

#endif