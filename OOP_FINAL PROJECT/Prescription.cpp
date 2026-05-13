#include "Prescription.h"
#include <cstring>

Prescription::Prescription(int id, int aId, int pId, int dId,
    const char* date, const char* medicines, const char* notes)
    : id(id), appointmentId(aId), patientId(pId), doctorId(dId) {

    this->date = new char[strlen(date) + 1];
    strcpy(this->date, date);

    this->medicines = new char[strlen(medicines) + 1];
    strcpy(this->medicines, medicines);

    this->notes = new char[strlen(notes) + 1];
    strcpy(this->notes, notes);
}

Prescription::Prescription(const Prescription& other)
    : id(other.id), appointmentId(other.appointmentId), patientId(other.patientId), doctorId(other.doctorId) {

    this->date = new char[strlen(other.date) + 1];
    strcpy(this->date, other.date);

    this->medicines = new char[strlen(other.medicines) + 1];
    strcpy(this->medicines, other.medicines);

    this->notes = new char[strlen(other.notes) + 1];
    strcpy(this->notes, other.notes);
}

Prescription& Prescription::operator=(const Prescription& other) {
    if (this != &other) {
        id = other.id;
        appointmentId = other.appointmentId;
        patientId = other.patientId;
        doctorId = other.doctorId;

        delete[] date;
        date = new char[strlen(other.date) + 1];
        strcpy(date, other.date);

        delete[] medicines;
        medicines = new char[strlen(other.medicines) + 1];
        strcpy(medicines, other.medicines);

        delete[] notes;
        notes = new char[strlen(other.notes) + 1];
        strcpy(notes, other.notes);
    }
    return *this;
}

Prescription::~Prescription() {
    delete[] date;
    delete[] medicines;
    delete[] notes;
}

int Prescription::getId() const { return id; }
int Prescription::getAppointmentId() const { return appointmentId; }
int Prescription::getPatientId() const { return patientId; }
int Prescription::getDoctorId() const { return doctorId; }
const char* Prescription::getDate() const { return date; }
const char* Prescription::getMedicines() const { return medicines; }
const char* Prescription::getNotes() const { return notes; }