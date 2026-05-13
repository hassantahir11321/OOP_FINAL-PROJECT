#include "Doctor.h"

Doctor::Doctor(int id, const char* name, const char* specialization,
    const char* contact, const char* password, double fee)
    : Person(id, name, password), fee(fee) {

    // Allocate and copy specialization
    this->specialization = new char[strlen(specialization) + 1];
    strcpy(this->specialization, specialization);

    // Allocate and copy contact
    this->contact = new char[strlen(contact) + 1];
    strcpy(this->contact, contact);
}

Doctor::Doctor(const Doctor& other)
    : Person(other), fee(other.fee) {

    this->specialization = new char[strlen(other.specialization) + 1];
    strcpy(this->specialization, other.specialization);

    this->contact = new char[strlen(other.contact) + 1];
    strcpy(this->contact, other.contact);
}

Doctor& Doctor::operator=(const Doctor& other) {
    if (this != &other) {
        Person::operator=(other); // Call base assignment

        fee = other.fee;

        delete[] specialization;
        this->specialization = new char[strlen(other.specialization) + 1];
        strcpy(this->specialization, other.specialization);

        delete[] contact;
        this->contact = new char[strlen(other.contact) + 1];
        strcpy(this->contact, other.contact);
    }
    return *this;
}

Doctor::~Doctor() {
    delete[] specialization;
    delete[] contact;
}

const char* Doctor::getSpecialization() const { return specialization; }
const char* Doctor::getContact() const { return contact; }
double Doctor::getFee() const { return fee; }

// Outputting the Doctor Menu required by the project PDF
void Doctor::displayMenu() const {
    std::cout << "\nWelcome, Dr. " << name << " | Specialization: " << specialization << "\n";
    std::cout << "1. View Today's Appointments\n";
    std::cout << "2. Mark Appointment Complete\n";
    std::cout << "3. Mark Appointment No-Show\n";
    std::cout << "4. Write Prescription\n";
    std::cout << "5. View Patient Medical History\n";
    std::cout << "6. Logout\n";
}

const char* Doctor::getRole() const {
    return "Doctor";
}

bool Doctor::operator==(const Doctor& other) const {
    return this->id == other.id;
}

std::ostream& operator<<(std::ostream& os, const Doctor& doctor) {
    os << "ID: " << doctor.id << " | Name: " << doctor.name
        << " | Specialization: " << doctor.specialization
        << " | Contact: " << doctor.contact << " | Fee: PKR " << doctor.fee;
    return os;
}