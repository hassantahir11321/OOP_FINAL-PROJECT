#include "FileHandler.h"
#include "HospitalException.h"
#include <fstream>
#include <iostream>

// ==========================================
// CUSTOM PARSING UTILITIES
// ==========================================

void FileHandler::extractField(const char* line, int& index, char* buffer) {
    int j = 0;
    while (line[index] != '\0' && line[index] != '\r' && line[index] != ',') {
        buffer[j++] = line[index++];
    }
    buffer[j] = '\0';
    if (line[index] == ',') {
        index++; // Move past the comma for the next extraction
    }
}

int FileHandler::parseInt(const char* str) {
    int res = 0;
    int i = 0;
    for (; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
            res = res * 10 + (str[i] - '0');
        }
    }
    return res;
}

double FileHandler::parseDouble(const char* str) {
    double res = 0;
    int i = 0;
    while (str[i] != '\0' && str[i] != '.') {
        if (str[i] >= '0' && str[i] <= '9') res = res * 10 + (str[i] - '0');
        i++;
    }
    if (str[i] == '.') {
        i++;
        double fraction = 1;
        while (str[i] != '\0') {
            if (str[i] >= '0' && str[i] <= '9') {
                fraction /= 10;
                res += (str[i] - '0') * fraction;
            }
            i++;
        }
    }
    return res;
}

// ==========================================
// PATIENT I/O
// ==========================================

void FileHandler::loadPatients(Storage<Patient>& storage) {
    std::ifstream file("patients.txt");
    if (!file.is_open()) return;

    char line[500];
    while (file.getline(line, 500)) {
        if (line[0] == '\0') continue; // Skip empty lines

        int idx = 0;
        char idStr[10], name[50], ageStr[10], genderStr[2], contact[15], password[20], balanceStr[20];

        extractField(line, idx, idStr);
        extractField(line, idx, name);
        extractField(line, idx, ageStr);
        extractField(line, idx, genderStr);
        extractField(line, idx, contact);
        extractField(line, idx, password);
        extractField(line, idx, balanceStr);

        storage.add(Patient(parseInt(idStr), name, parseInt(ageStr), genderStr[0], contact, password, parseDouble(balanceStr)));
    }
    file.close();
}

void FileHandler::savePatient(const Patient& p) {
    std::ofstream file("patients.txt", std::ios::app);
    if (file.is_open()) {
        file << p.getId() << "," << p.getName() << "," << p.getAge() << ","
            << p.getGender() << "," << p.getContact() << "," << p.getPassword() << ","
            << p.getBalance() << "\n";
        file.close();
    }
}

void FileHandler::rewritePatients(Storage<Patient>& storage) {
    std::ofstream file("patients.txt", std::ios::trunc); // trunc clears the file
    if (file.is_open()) {
        Patient* arr = storage.getAll();
        for (int i = 0; i < storage.size(); i++) {
            file << arr[i].getId() << "," << arr[i].getName() << "," << arr[i].getAge() << ","
                << arr[i].getGender() << "," << arr[i].getContact() << "," << arr[i].getPassword() << ","
                << arr[i].getBalance() << "\n";
        }
        file.close();
    }
}

// ==========================================
// DOCTOR I/O
// ==========================================

void FileHandler::loadDoctors(Storage<Doctor>& storage) {
    std::ifstream file("doctors.txt");
    if (!file.is_open()) return;

    char line[500];
    while (file.getline(line, 500)) {
        if (line[0] == '\0') continue;

        int idx = 0;
        char idStr[10], name[50], spec[50], contact[15], pass[20], feeStr[20];

        extractField(line, idx, idStr);
        extractField(line, idx, name);
        extractField(line, idx, spec);
        extractField(line, idx, contact);
        extractField(line, idx, pass);
        extractField(line, idx, feeStr);

        storage.add(Doctor(parseInt(idStr), name, spec, contact, pass, parseDouble(feeStr)));
    }
    file.close();
}

void FileHandler::saveDoctor(const Doctor& d) {
    std::ofstream file("doctors.txt", std::ios::app);
    if (file.is_open()) {
        file << d.getId() << "," << d.getName() << "," << d.getSpecialization() << ","
            << d.getContact() << "," << d.getPassword() << "," << d.getFee() << "\n";
        file.close();
    }
}

void FileHandler::rewriteDoctors(Storage<Doctor>& storage) {
    std::ofstream file("doctors.txt", std::ios::trunc);
    if (file.is_open()) {
        Doctor* arr = storage.getAll();
        for (int i = 0; i < storage.size(); i++) {
            file << arr[i].getId() << "," << arr[i].getName() << "," << arr[i].getSpecialization() << ","
                << arr[i].getContact() << "," << arr[i].getPassword() << "," << arr[i].getFee() << "\n";
        }
        file.close();
    }
}

// ==========================================
// ADMIN I/O
// ==========================================

void FileHandler::loadAdmins(Storage<Admin>& storage) {
    std::ifstream file("admin.txt");
    if (!file.is_open()) throw FileNotFoundException(); // Admin file MUST exist on startup

    char line[500];
    while (file.getline(line, 500)) {
        if (line[0] == '\0') continue;

        int idx = 0;
        char idStr[10], name[50], pass[50];

        extractField(line, idx, idStr);
        extractField(line, idx, name);
        extractField(line, idx, pass);

        storage.add(Admin(parseInt(idStr), name, pass));
    }
    file.close();
}

// ==========================================
// APPOINTMENT I/O
// ==========================================

void FileHandler::loadAppointments(Storage<Appointment>& storage) {
    std::ifstream file("appointments.txt");
    if (!file.is_open()) return;

    char line[500];
    while (file.getline(line, 500)) {
        if (line[0] == '\0') continue;

        int idx = 0;
        char idStr[10], pIdStr[10], dIdStr[10], date[15], time[10], status[20];

        extractField(line, idx, idStr);
        extractField(line, idx, pIdStr);
        extractField(line, idx, dIdStr);
        extractField(line, idx, date);
        extractField(line, idx, time);
        extractField(line, idx, status);

        storage.add(Appointment(parseInt(idStr), parseInt(pIdStr), parseInt(dIdStr), date, time, status));
    }
    file.close();
}

void FileHandler::saveAppointment(const Appointment& a) {
    std::ofstream file("appointments.txt", std::ios::app);
    if (file.is_open()) {
        file << a.getId() << "," << a.getPatientId() << "," << a.getDoctorId() << ","
            << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
        file.close();
    }
}

void FileHandler::rewriteAppointments(Storage<Appointment>& storage) {
    std::ofstream file("appointments.txt", std::ios::trunc);
    if (file.is_open()) {
        Appointment* arr = storage.getAll();
        for (int i = 0; i < storage.size(); i++) {
            file << arr[i].getId() << "," << arr[i].getPatientId() << "," << arr[i].getDoctorId() << ","
                << arr[i].getDate() << "," << arr[i].getTimeSlot() << "," << arr[i].getStatus() << "\n";
        }
        file.close();
    }
}

// ==========================================
// BILL I/O
// ==========================================

void FileHandler::loadBills(Storage<Bill>& storage) {
    std::ifstream file("bills.txt");
    if (!file.is_open()) return;

    char line[500];
    while (file.getline(line, 500)) {
        if (line[0] == '\0') continue;

        int idx = 0;
        char idStr[10], pIdStr[10], aIdStr[10], amtStr[20], status[20], date[15];

        extractField(line, idx, idStr);
        extractField(line, idx, pIdStr);
        extractField(line, idx, aIdStr);
        extractField(line, idx, amtStr);
        extractField(line, idx, status);
        extractField(line, idx, date);

        storage.add(Bill(parseInt(idStr), parseInt(pIdStr), parseInt(aIdStr), parseDouble(amtStr), status, date));
    }
    file.close();
}

void FileHandler::saveBill(const Bill& b) {
    std::ofstream file("bills.txt", std::ios::app);
    if (file.is_open()) {
        file << b.getId() << "," << b.getPatientId() << "," << b.getAppointmentId() << ","
            << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << "\n";
        file.close();
    }
}

void FileHandler::rewriteBills(Storage<Bill>& storage) {
    std::ofstream file("bills.txt", std::ios::trunc);
    if (file.is_open()) {
        Bill* arr = storage.getAll();
        for (int i = 0; i < storage.size(); i++) {
            file << arr[i].getId() << "," << arr[i].getPatientId() << "," << arr[i].getAppointmentId() << ","
                << arr[i].getAmount() << "," << arr[i].getStatus() << "," << arr[i].getDate() << "\n";
        }
        file.close();
    }
}

// ==========================================
// PRESCRIPTION I/O
// ==========================================

void FileHandler::loadPrescriptions(Storage<Prescription>& storage) {
    std::ifstream file("prescriptions.txt");
    if (!file.is_open()) return;

    char line[1000];
    while (file.getline(line, 1000)) {
        if (line[0] == '\0') continue;

        int idx = 0;
        char idStr[10], aIdStr[10], pIdStr[10], dIdStr[10], date[15], meds[500], notes[300];

        extractField(line, idx, idStr);
        extractField(line, idx, aIdStr);
        extractField(line, idx, pIdStr);
        extractField(line, idx, dIdStr);
        extractField(line, idx, date);
        extractField(line, idx, meds);
        extractField(line, idx, notes);

        storage.add(Prescription(parseInt(idStr), parseInt(aIdStr), parseInt(pIdStr), parseInt(dIdStr), date, meds, notes));
    }
    file.close();
}

void FileHandler::savePrescription(const Prescription& p) {
    std::ofstream file("prescriptions.txt", std::ios::app);
    if (file.is_open()) {
        file << p.getId() << "," << p.getAppointmentId() << "," << p.getPatientId() << "," << p.getDoctorId() << ","
            << p.getDate() << "," << p.getMedicines() << "," << p.getNotes() << "\n";
        file.close();
    }
}

// ==========================================
// SECURITY & ARCHIVING
// ==========================================

void FileHandler::logSecurityEvent(const char* timestamp, const char* role, const char* enteredId, const char* result) {
    std::ofstream file("security_log.txt", std::ios::app);
    if (file.is_open()) {
        file << timestamp << "," << role << "," << enteredId << "," << result << "\n";
        file.close();
    }
}

void FileHandler::archiveDischargedPatient(const char* patientData) {
    std::ofstream file("discharged.txt", std::ios::app);
    if (file.is_open()) {
        file << patientData << "\n";
        file.close();
    }
}