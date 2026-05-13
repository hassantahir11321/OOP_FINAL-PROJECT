#include "Bill.h"
#include <cstring>

// --- Constructor ---
Bill::Bill(int id, int patientId, int appointmentId, double amount, const char* status, const char* date) {
    this->id = id;
    this->patientId = patientId;
    this->appointmentId = appointmentId;
    this->amount = amount;

    // Dynamically allocate memory for strings to match your constraints
    this->status = new char[strlen(status) + 1];
    strcpy(this->status, status);

    this->date = new char[strlen(date) + 1];
    strcpy(this->date, date);
}

// --- Copy Constructor ---
Bill::Bill(const Bill& other) {
    this->id = other.id;
    this->patientId = other.patientId;             
    this->appointmentId = other.appointmentId;
    this->amount = other.amount;

    this->status = new char[strlen(other.status) + 1];
    strcpy(this->status, other.status);

    this->date = new char[strlen(other.date) + 1];
    strcpy(this->date, other.date);
}

// --- Assignment Operator ---
Bill& Bill::operator=(const Bill& other) {
    if (this != &other) {
        delete[] this->status;
        delete[] this->date;

        this->id = other.id;
        this->patientId = other.patientId;
        this->appointmentId = other.appointmentId;
        this->amount = other.amount;

        this->status = new char[strlen(other.status) + 1];
        strcpy(this->status, other.status);

        this->date = new char[strlen(other.date) + 1];
        strcpy(this->date, other.date);
    }
    return *this;
}

// --- Destructor ---
Bill::~Bill() {
    delete[] status;
    delete[] date;
}

// --- Getters ---
int Bill::getId() const { return id; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
double Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status; }
const char* Bill::getDate() const { return date; }