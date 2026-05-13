#include "Person.h"

Person::Person(int id, const char* pName, const char* pPassword) : id(id) {
    // Dynamically allocate memory to adhere to the strict dynamic pointer rule
    name = new char[strlen(pName) + 1];
    strcpy(name, pName);

    password = new char[strlen(pPassword) + 1];
    strcpy(password, pPassword);
}

Person::Person(const Person& other) : id(other.id) {
    name = new char[strlen(other.name) + 1];
    strcpy(name, other.name);

    password = new char[strlen(other.password) + 1];
    strcpy(password, other.password);
}

Person& Person::operator=(const Person& other) {
    if (this != &other) {
        id = other.id;

        delete[] name;
        name = new char[strlen(other.name) + 1];
        strcpy(name, other.name);

        delete[] password;
        password = new char[strlen(other.password) + 1];
        strcpy(password, other.password);
    }
    return *this;
}

Person::~Person() {
    delete[] name;
    delete[] password;
}

int Person::getId() const { return id; }
const char* Person::getName() const { return name; }
const char* Person::getPassword() const { return password; }
