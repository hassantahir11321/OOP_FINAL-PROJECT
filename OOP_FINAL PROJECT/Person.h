#ifndef PERSON_H
#define PERSON_H

#include <iostream>
#include <cstring>

class Person {
protected:
    int id;
    char* name;
    char* password;

public:
    // Constructor
    Person(int id, const char* pName, const char* pPassword);

    // Copy Constructor (Rule of Three necessity for dynamic memory)
    Person(const Person& other);

    // Assignment Operator
    Person& operator=(const Person& other);

    // Virtual Destructor to ensure proper cleanup of derived classes
    virtual ~Person();

    // Getters
    int getId() const;
    const char* getName() const;
    const char* getPassword() const;

    // Two pure virtual methods making this class Abstract
    virtual void displayMenu() const = 0;
    virtual const char* getRole() const = 0;
};

#endif