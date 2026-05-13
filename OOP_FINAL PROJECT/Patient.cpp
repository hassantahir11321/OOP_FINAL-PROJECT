#include "Patient.h"

Patient::Patient(int id, const char* name, int age, char gender,
    const char* contact, const char* password, double balance)
    : Person(id, name, password), age(age), gender(gender), balance(balance) {

    // Dynamically allocate memory for contact
    this->contact = new char[strlen(contact) + 1];
    strcpy(this->contact, contact);
}

Patient::Patient(const Patient& other)
    : Person(other), age(other.age), gender(other.gender), balance(other.balance) {

    this->contact = new char[strlen(other.contact) + 1];
    strcpy(this->contact, other.contact);
}

Patient& Patient::operator=(const Patient& other) {
    if (this != &other) {
        Person::operator=(other); // Call the base class assignment operator first

        age = other.age;
        gender = other.gender;
        balance = other.balance;

        delete[] contact; // Free old memory
        contact = new char[strlen(other.contact) + 1];
        strcpy(contact, other.contact);
    }
    return *this;
}

Patient::~Patient() {
    delete[] contact;
}

int Patient::getAge() const { return age; }
char Patient::getGender() const { return gender; }
const char* Patient::getContact() const { return contact; }
double Patient::getBalance() const { return balance; }

// Fulfilling the Patient Menu UI requirement 
void Patient::displayMenu() const {
    std::cout << "\nWelcome, " << name << "\n";
    std::cout << "Balance: PKR " << balance << "\n";
    std::cout << "1. Book Appointment\n";
    std::cout << "2. Cancel Appointment\n";
    std::cout << "3. View My Appointments\n";
    std::cout << "4. View My Medical Records\n";
    std::cout << "5. View My Bills\n";
    std::cout << "6. Pay Bill\n";
    std::cout << "7. Top Up Balance\n";
    std::cout << "8. Logout\n";
}

const char* Patient::getRole() const {
    return "Patient";
}

// --- Operator Overloads Implementation ---

Patient& Patient::operator+=(double amount) {
    this->balance += amount;
    return *this;
}

Patient& Patient::operator-=(double amount) {
    this->balance -= amount;
    return *this;
}

bool Patient::operator==(const Patient& other) const {
    return this->id == other.id;
}

std::ostream& operator<<(std::ostream& os, const Patient& patient) {
    os << "ID: " << patient.id << " | Name: " << patient.name
        << " | Age: " << patient.age << " | Gender: " << patient.gender
        << " | Contact: " << patient.contact << " | Balance: PKR " << patient.balance;
    return os;
}