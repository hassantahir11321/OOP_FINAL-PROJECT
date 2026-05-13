#include "Admin.h"
#include <iostream>

Admin::Admin(int id, const char* name, const char* password)
    : Person(id, name, password) {
    // No extra dynamic memory needed, Person handles name and password!
}

void Admin::displayMenu() const {
    std::cout << "\nAdmin Panel - MediCore\n";
    std::cout << "1. Add Doctor\n";
    std::cout << "2. Remove Doctor\n";
    std::cout << "3. View All Patients\n";
    std::cout << "4. View All Doctors\n";
    std::cout << "5. View All Appointments\n";
    std::cout << "6. View Unpaid Bills\n";
    std::cout << "7. Discharge Patient\n";
    std::cout << "8. View Security Log\n";
    std::cout << "9. Generate Daily Report\n";
    std::cout << "10. Logout\n";
}

const char* Admin::getRole() const {
    return "Admin";
}