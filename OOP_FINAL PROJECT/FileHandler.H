#pragma once
#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "Storage.h"

class FileHandler {
private:
    // Custom utility to extract data between commas
    static void extractField(const char* line, int& index, char* buffer);

    // Custom number parsers to avoid built-in string/math functions
    static int parseInt(const char* str);
    static double parseDouble(const char* str);

public:
    // --- Patient I/O ---
    static void loadPatients(Storage<Patient>& storage);
    static void savePatient(const Patient& patient);
    static void rewritePatients(Storage<Patient>& storage); // Used for updates/deletes

    // --- Doctor I/O ---
    static void loadDoctors(Storage<Doctor>& storage);
    static void saveDoctor(const Doctor& doctor);
    static void rewriteDoctors(Storage<Doctor>& storage);

    // --- Admin I/O ---
    static void loadAdmins(Storage<Admin>& storage);

    // --- Appointment I/O ---
    static void loadAppointments(Storage<Appointment>& storage);
    static void saveAppointment(const Appointment& appt);
    static void rewriteAppointments(Storage<Appointment>& storage);

    // --- Bill I/O ---
    static void loadBills(Storage<Bill>& storage);
    static void saveBill(const Bill& bill);
    static void rewriteBills(Storage<Bill>& storage);

    // --- Prescription I/O ---
    static void loadPrescriptions(Storage<Prescription>& storage);
    static void savePrescription(const Prescription& pres);

    // --- Security & Archiving ---
    static void logSecurityEvent(const char* timestamp, const char* role, const char* enteredId, const char* result);
    static void archiveDischargedPatient(const char* patientData);
};

#endif