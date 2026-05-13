#include "Appointment.h"
#include <cstring>

Appointment::Appointment(int id, int pId, int dId, const char* date,
    const char* timeSlot, const char* status)
    : id(id), patientId(pId), doctorId(dId) {

    this->date = new char[strlen(date) + 1];
    strcpy(this->date, date);

    this->timeSlot = new char[strlen(timeSlot) + 1];
    strcpy(this->timeSlot, timeSlot);

    this->status = new char[strlen(status) + 1];
    strcpy(this->status, status);
}

Appointment::Appointment(const Appointment& other)
    : id(other.id), patientId(other.patientId), doctorId(other.doctorId) {

    this->date = new char[strlen(other.date) + 1];
    strcpy(this->date, other.date);

    this->timeSlot = new char[strlen(other.timeSlot) + 1];
    strcpy(this->timeSlot, other.timeSlot);

    this->status = new char[strlen(other.status) + 1];
    strcpy(this->status, other.status);
}

Appointment& Appointment::operator=(const Appointment& other) {
    if (this != &other) {
        id = other.id;
        patientId = other.patientId;
        doctorId = other.doctorId;

        delete[] date;
        date = new char[strlen(other.date) + 1];
        strcpy(date, other.date);

        delete[] timeSlot;
        timeSlot = new char[strlen(other.timeSlot) + 1];
        strcpy(timeSlot, other.timeSlot);

        delete[] status;
        status = new char[strlen(other.status) + 1];
        strcpy(status, other.status);
    }
    return *this;
}

Appointment::~Appointment() {
    delete[] date;
    delete[] timeSlot;
    delete[] status;
}

int Appointment::getId() const { return id; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }

void Appointment::setStatus(const char* newStatus) {
    delete[] status;
    status = new char[strlen(newStatus) + 1];
    strcpy(status, newStatus);
}

// Conflict check: same doctor, same date, same time slot, neither cancelled
bool Appointment::operator==(const Appointment& other) const {
    if (this->doctorId == other.doctorId &&
        strcmp(this->date, other.date) == 0 &&
        strcmp(this->timeSlot, other.timeSlot) == 0) {

        if (strcmp(this->status, "cancelled") != 0 && strcmp(other.status, "cancelled") != 0) {
            return true; // A conflict exists
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const Appointment& appt) {
    os << "Appt ID: " << appt.id << " | Patient ID: " << appt.patientId
        << " | Doctor ID: " << appt.doctorId << " | Date: " << appt.date
        << " | Time: " << appt.timeSlot << " | Status: " << appt.status;
    return os;
}