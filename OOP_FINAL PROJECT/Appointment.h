#pragma once
#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iostream>

class Appointment {
private:
    int id;
    int patientId;
    int doctorId;
    char* date;
    char* timeSlot;
    char* status; // pending, completed, no-show, cancelled

public:
    Appointment(int id = 0, int pId = 0, int dId = 0, const char* date = "",
        const char* timeSlot = "", const char* status = "");
    Appointment(const Appointment& other);
    Appointment& operator=(const Appointment& other);
    ~Appointment();

    int getId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;

    void setStatus(const char* newStatus);

    // Required by assignment: Checks if two appointments conflict
    bool operator==(const Appointment& other) const;

    // Required by assignment: Formatted display
    friend std::ostream& operator<<(std::ostream& os, const Appointment& appt);
};

#endif