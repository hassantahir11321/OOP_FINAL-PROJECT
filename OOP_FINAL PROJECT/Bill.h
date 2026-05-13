#pragma once
#ifndef BILL_H
#define BILL_H

class Bill {
private:
    int id;
    int patientId;
    int appointmentId;
    double amount;
    char* status; // paid, unpaid, cancelled
    char* date;

public:
    Bill(int id = 0, int pId = 0, int aId = 0, double amount = 0.0,
        const char* status = "", const char* date = "");
    Bill(const Bill& other);
    Bill& operator=(const Bill& other);
    ~Bill();

    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    double getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;

    void setStatus(const char* newStatus);
};

#endif