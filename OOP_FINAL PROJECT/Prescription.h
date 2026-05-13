#pragma once
#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

class Prescription {
private:
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    char* date;
    char* medicines;
    char* notes;

public:
    Prescription(int id = 0, int aId = 0, int pId = 0, int dId = 0,
        const char* date = "", const char* medicines = "", const char* notes = "");
    Prescription(const Prescription& other);
    Prescription& operator=(const Prescription& other);
    ~Prescription();

    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;
};

#endif