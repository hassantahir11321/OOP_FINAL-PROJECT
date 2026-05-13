# MediCore - Hospital Management System 🏥

MediCore is a comprehensive, desktop-based Hospital Management System built from scratch in **C++**. It features a fully custom graphical user interface powered by **SFML**, completely bypassing the standard console terminal for a modern user experience. 

This project was developed as a Final Project for Object-Oriented Programming (OOP) to demonstrate advanced backend data management and memory control architectures.

## ✨ Key Features
* **Secure Admin Login:** Validates credentials against persistent local storage.
* **Graphical Dashboard:** Interactive, dark-themed UI with mouse-click and keyboard-tab state navigation.
* **Doctor Directory:** Add, remove, and search for doctors by name or ID in real-time.
* **Patient Records:** Manage patient demographics, balances, and conditions.
* **Appointment Scheduling:** Link specific patients to specific doctors with times and dates.
* **Billing System:** Generate invoices, track amounts, and calculate total pending balances.

## 🧠 Core OOP Concepts Implemented
Because this system is built without the standard `<string>` library, it relies on strict C++ memory management and core OOP principles:
* **Dynamic Memory & The Rule of Three:** Uses raw `char*` arrays allocated on the heap, safely managed via custom Destructors, Copy Constructors, and Assignment Operators (Deep Copying).
* **Inheritance:** Utilizes a base `Person` class to share common attributes (`id`, `name`, `password`) with derived `Doctor`, `Patient`, and `Admin` classes.
* **Polymorphism:** Implements both runtime polymorphism (virtual functions for specific user menus) and compile-time polymorphism (operator overloading for `==` comparisons and `<<` object printing).
* **Templates:** Employs a generic `Storage<T>` template class to handle dynamic arrays of any object type, preventing code duplication.
* **Encapsulation:** Protects all entity states, requiring data modification through secure setter/getter methods and overloaded operators (e.g., `operator+=` for patient balances).

## 🛠️ Tech Stack & Requirements
* **Language:** C++ (C++11 or higher)
* **Graphics Library:** SFML (Simple and Fast Multimedia Library) v2.5+
* **IDE:** Visual Studio / Any C++ Compiler

## 🚀 How to Run
1. Clone the repository to your local machine.
2. Ensure **SFML** is configured in your C++ IDE (Include directories, Library directories, and Linker dependencies).
3. Place the provided `.ttf` font file and `.txt` data files in the same directory as the executable.
4. Build and Run `Main.cpp`.

---
*Developed by [Your Name] for the FAST NU LHR OOP Final Project.*
