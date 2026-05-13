#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cstring>
#include <string>
#include <sstream>

// --- BACKEND HEADERS ---
#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"
#include "FileHandler.h"
#include "Validator.h"
#include "HospitalException.h"

using namespace std;
using namespace sf;

// ============================================================
// SCREEN STATE CONSTANTS
// ============================================================
const int STATE_LOGIN = 0;
const int STATE_DASHBOARD = 1;
const int STATE_DOCTORS = 2;
const int STATE_PATIENTS = 3;
const int STATE_APPOINTMENTS = 4;
const int STATE_BILLING = 5;
const int STATE_ADD_DOCTOR = 6;
const int STATE_ADD_PATIENT = 7;
const int STATE_ADD_APPOINTMENT = 8;
const int STATE_ADD_BILL = 9;
const int STATE_SEARCH_DOCTOR = 10;
const int STATE_SEARCH_PATIENT = 11;
const int STATE_VIEW_DOCTOR = 12;
const int STATE_VIEW_PATIENT = 13;

// ============================================================
// THEME COLORS
// ============================================================
const Color BG_DARK(10, 18, 24);
const Color BG_PANEL(15, 28, 38);
const Color TEAL(0, 210, 180);
const Color TEAL_DIM(0, 140, 120);
const Color TEAL_DARK(0, 60, 55);
const Color CYAN_BRIGHT(100, 240, 255);
const Color TEXT_WHITE(220, 235, 240);
const Color TEXT_GREY(120, 145, 155);
const Color RED_ALERT(220, 60, 60);
const Color GREEN_OK(60, 200, 100);
const Color GOLD(255, 200, 60);

// ============================================================
// HELPER: Draw a styled panel (rounded-look rectangle)
// ============================================================
void drawPanel(RenderWindow& window, float x, float y, float w, float h, Color fill, Color outline, float thickness = 1.5f) {
    RectangleShape panel(Vector2f(w, h));
    panel.setPosition(x, y);
    panel.setFillColor(fill);
    panel.setOutlineColor(outline);
    panel.setOutlineThickness(thickness);
    window.draw(panel);
}

// ============================================================
// HELPER: Draw a labeled text field box
// ============================================================
void drawField(RenderWindow& window, Font& font, float x, float y, float w,
    const string& label, const string& value, bool focused) {
    Color outline = focused ? TEAL : Color(40, 70, 80);
    drawPanel(window, x, y, w, 36.f, Color(12, 25, 32), outline, focused ? 2.f : 1.f);

    Text lbl(label, font, 13);
    lbl.setFillColor(TEXT_GREY);
    lbl.setPosition(x, y - 18.f);
    window.draw(lbl);

    Text val(value, font, 17);
    val.setFillColor(focused ? CYAN_BRIGHT : TEXT_WHITE);
    val.setPosition(x + 8.f, y + 7.f);
    window.draw(val);

    if (focused) {
        // Blinking cursor effect via a thin rect
        float cx = x + 10.f + val.getGlobalBounds().width;
        RectangleShape cursor(Vector2f(2.f, 20.f));
        cursor.setFillColor(TEAL);
        cursor.setPosition(cx, y + 8.f);
        window.draw(cursor);
    }
}

// ============================================================
// HELPER: Draw a fancy action button
// ============================================================
void drawButton(RenderWindow& window, Font& font, RectangleShape& btn,
    const string& label, unsigned int fontSize = 18) {
    window.draw(btn);
    Text t(label, font, fontSize);
    FloatRect tb = t.getLocalBounds();
    FloatRect bb = btn.getGlobalBounds();
    t.setPosition(bb.left + (bb.width - tb.width) / 2.f - tb.left,
        bb.top + (bb.height - tb.height) / 2.f - tb.top);
    t.setFillColor(TEXT_WHITE);
    window.draw(t);
}

// ============================================================
// HELPER: Draw a horizontal divider line
// ============================================================
void drawDivider(RenderWindow& window, float x, float y, float w) {
    RectangleShape line(Vector2f(w, 1.f));
    line.setPosition(x, y);
    line.setFillColor(TEAL_DARK);
    window.draw(line);
}

// ============================================================
// HELPER: Draw screen title with accent bar
// ============================================================
void drawTitle(RenderWindow& window, Font& font, const string& title, const string& subtitle = "") {
    // Accent bar
    RectangleShape bar(Vector2f(4.f, subtitle.empty() ? 40.f : 55.f));
    bar.setPosition(28.f, 22.f);
    bar.setFillColor(TEAL);
    window.draw(bar);

    Text t(title, font, 28);
    t.setFillColor(CYAN_BRIGHT);
    t.setPosition(42.f, 20.f);
    window.draw(t);

    if (!subtitle.empty()) {
        Text sub(subtitle, font, 14);
        sub.setFillColor(TEXT_GREY);
        sub.setPosition(43.f, 52.f);
        window.draw(sub);
    }
}

// ============================================================
// HELPER: Draw status message bar at bottom
// ============================================================
void drawStatusBar(RenderWindow& window, Font& font, const string& msg, bool isError = false) {
    drawPanel(window, 0.f, 565.f, 800.f, 35.f, Color(8, 20, 28), isError ? RED_ALERT : TEAL_DARK, 0.f);
    Text t(msg, font, 14);
    t.setFillColor(isError ? Color(255, 180, 180) : Color(160, 230, 220));
    t.setPosition(20.f, 572.f);
    window.draw(t);
}

// ============================================================
// HELPER: Draw a scrollable record list row
// ============================================================
void drawRecord(RenderWindow& window, Font& font, float y, int index,
    const string& col1, const string& col2, const string& col3,
    bool highlight = false) {
    Color rowBg = highlight ? Color(0, 50, 45) : (index % 2 == 0 ? Color(12, 22, 30) : Color(15, 28, 38));
    drawPanel(window, 25.f, y, 750.f, 32.f, rowBg, Color(20, 45, 55), 0.5f);

    auto makeText = [&](const string& s, float x, unsigned int sz, Color c) {
        Text t(s, font, sz);
        t.setFillColor(c);
        t.setPosition(x, y + 7.f);
        window.draw(t);
        };

    makeText(col1, 35.f, 15, TEXT_WHITE);
    makeText(col2, 250.f, 15, TEXT_GREY);
    makeText(col3, 530.f, 15, TEAL);
}

// ============================================================
// HELPER: Create a styled button rectangle
// ============================================================
RectangleShape makeBtn(float x, float y, float w, float h,
    Color fill = Color(0, 80, 75)) {
    RectangleShape btn(Vector2f(w, h));
    btn.setPosition(x, y);
    btn.setFillColor(fill);
    btn.setOutlineColor(TEAL_DIM);
    btn.setOutlineThickness(1.f);
    return btn;
}

// ============================================================
// MULTI-FIELD FORM HELPER STRUCT REPLACEMENT
// We use parallel arrays to manage form fields without structs/enums
// ============================================================
const int MAX_FORM_FIELDS = 6;
char   formValues[MAX_FORM_FIELDS][100];
char   formLabels[MAX_FORM_FIELDS][60];
int    formLengths[MAX_FORM_FIELDS];
int    formCount = 0;
int    focusedField = 0;

void initForm(int count) {
    formCount = count;
    focusedField = 0;
    for (int i = 0; i < count; i++) {
        formValues[i][0] = '\0';
        formLengths[i] = 0;
    }
}

void handleFormText(unsigned int unicode) {
    if (focusedField < 0 || focusedField >= formCount) return;
    if (unicode == 8) {
        if (formLengths[focusedField] > 0) {
            formValues[focusedField][--formLengths[focusedField]] = '\0';
        }
    }
    else if (unicode == 9) { // Tab — advance field
        focusedField = (focusedField + 1) % formCount;
    }
    else if (formLengths[focusedField] < 98 && unicode >= 32 && unicode <= 126) {
        formValues[focusedField][formLengths[focusedField]++] = (char)unicode;
        formValues[focusedField][formLengths[focusedField]] = '\0';
    }
}

void drawForm(RenderWindow& window, Font& font, float startY, float colX = 80.f, float fieldW = 300.f) {
    for (int i = 0; i < formCount; i++) {
        float y = startY + i * 70.f;
        drawField(window, font, colX, y, fieldW,
            formLabels[i], string(formValues[i]), focusedField == i);
    }
}

// ============================================================
// MAIN
// ============================================================
int main() {
    // --- BACKEND INIT ---
    Storage<Patient>     patients;
    Storage<Doctor>      doctors;
    Storage<Admin>       admins;
    Storage<Appointment> appointments;
    Storage<Bill>        bills;
    Storage<Prescription>prescriptions;

    try {
        FileHandler::loadAdmins(admins);
        FileHandler::loadPatients(patients);
        FileHandler::loadDoctors(doctors);
        FileHandler::loadAppointments(appointments);
        FileHandler::loadBills(bills);
        FileHandler::loadPrescriptions(prescriptions);
    }
    catch (...) {
        cout << "Warning: Some data files could not be loaded." << endl;
    }

    // --- WINDOW ---
    RenderWindow window(VideoMode(1366, 768), "MediCore HMS  |  Hospital Management System", Style::Default);
    View view(FloatRect(0.f, 0.f, 800.f, 600.f));
    window.setView(view);
    window.setFramerateLimit(60);

    Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    int  currentState = STATE_LOGIN;
    bool errorState = false;
    string statusMsg = "Welcome — please log in.";
    int  scrollOffset = 0; // for list scrolling
    int  selectedIndex = -1;

    // Search buffers
    char searchBuf[100] = ""; int searchLen = 0;
    bool searchFocused = false;

    // ---- LOGIN FIELDS ----
    char idInput[50] = ""; int idLength = 0;
    char passInput[50] = ""; int passLength = 0;
    char passDisplay[50] = "";
    bool isIdFocused = true;
    bool isPassFocused = false;

    // ---- COMMON BUTTONS ----
    RectangleShape backBtn = makeBtn(680.f, 12.f, 100.f, 34.f, Color(20, 45, 55));
    RectangleShape addBtn = makeBtn(25.f, 80.f, 140.f, 34.f);
    RectangleShape srchBtn = makeBtn(180.f, 80.f, 140.f, 34.f, Color(20, 55, 75));
    RectangleShape delBtn = makeBtn(335.f, 80.f, 140.f, 34.f, Color(80, 20, 20));
    RectangleShape saveBtn = makeBtn(250.f, 460.f, 180.f, 42.f);
    RectangleShape cancelBtn = makeBtn(450.f, 460.f, 130.f, 42.f, Color(60, 20, 20));

    // ---- LOGIN SCREEN ----
    RectangleShape loginCard(Vector2f(380.f, 320.f));
    loginCard.setPosition(210.f, 130.f);
    loginCard.setFillColor(BG_PANEL);
    loginCard.setOutlineColor(TEAL_DARK);
    loginCard.setOutlineThickness(1.5f);

    RectangleShape loginBtn = makeBtn(265.f, 370.f, 270.f, 48.f);

    // ---- DASHBOARD BUTTONS ----
    float dashX = 120.f, dashY = 140.f, dashW = 240.f, dashH = 56.f, dashGap = 72.f;
    RectangleShape dBtns[4];
    string dLabels[4] = { "Doctors", "Patients", "Appointments", "Billing" };
    string dIcons[4] = { "[ Dr ]", "[ Pt ]", "[ Ap ]", "[ Bl ]" };
    for (int i = 0; i < 4; i++) {
        int col = i % 2, row = i / 2;
        dBtns[i] = makeBtn(dashX + col * 280.f, dashY + row * 80.f, dashW, dashH);
    }
    RectangleShape logoutBtn = makeBtn(280.f, 490.f, 240.f, 46.f, Color(70, 15, 15));

    // ==========================================
    // MAIN LOOP
    // ==========================================
    Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        (void)dt;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
            if (event.type == Event::Resized) {
                View view(FloatRect(0.f, 0.f, 800.f, 600.f));
                window.setView(view);
            }

            // ---- SCROLL ----
            if (event.type == Event::MouseWheelScrolled) {
                scrollOffset -= (int)event.mouseWheelScroll.delta * 2;
                if (scrollOffset < 0) scrollOffset = 0;
            }

            // ---- MOUSE CLICK ----
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                Vector2f mp = window.mapPixelToCoords(Mouse::getPosition(window));
                errorState = false;

                // ---- GLOBAL BACK BUTTON ----
                if (currentState != STATE_LOGIN && currentState != STATE_DASHBOARD) {
                    if (backBtn.getGlobalBounds().contains(mp)) {
                        if (currentState == STATE_ADD_DOCTOR || currentState == STATE_SEARCH_DOCTOR || currentState == STATE_VIEW_DOCTOR)   currentState = STATE_DOCTORS;
                        else if (currentState == STATE_ADD_PATIENT || currentState == STATE_SEARCH_PATIENT || currentState == STATE_VIEW_PATIENT) currentState = STATE_PATIENTS;
                        else if (currentState == STATE_ADD_APPOINTMENT) currentState = STATE_APPOINTMENTS;
                        else if (currentState == STATE_ADD_BILL)        currentState = STATE_BILLING;
                        else currentState = STATE_DASHBOARD;
                        scrollOffset = 0; selectedIndex = -1;
                        statusMsg = ""; errorState = false;
                        continue;
                    }
                }

                // ---- LOGIN ----
                if (currentState == STATE_LOGIN) {
                    if (loginCard.getGlobalBounds().contains(mp)) {
                        isIdFocused = (mp.y < 310.f);
                        isPassFocused = !isIdFocused;
                    }
                    if (loginBtn.getGlobalBounds().contains(mp)) {
                        if (Validator::validateAdmin(idInput, passInput, admins)) {
                            currentState = STATE_DASHBOARD;
                            statusMsg = "Login successful. Welcome, Admin.";
                            errorState = false;
                        }
                        else {
                            statusMsg = "Invalid ID or password. Try again.";
                            errorState = true;
                        }
                    }
                }

                // ---- DASHBOARD ----
                else if (currentState == STATE_DASHBOARD) {
                    for (int i = 0; i < 4; i++) {
                        if (dBtns[i].getGlobalBounds().contains(mp)) {
                            currentState = STATE_DOCTORS + i;
                            scrollOffset = 0; selectedIndex = -1;
                            statusMsg = "";
                        }
                    }
                    if (logoutBtn.getGlobalBounds().contains(mp)) {
                        idLength = passLength = 0;
                        idInput[0] = passInput[0] = passDisplay[0] = '\0';
                        isIdFocused = true; isPassFocused = false;
                        currentState = STATE_LOGIN;
                        statusMsg = "Logged out successfully.";
                    }
                }

                // ---- DOCTOR LIST ----
                else if (currentState == STATE_DOCTORS) {
                    if (addBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_ADD_DOCTOR;
                        strcpy(formLabels[0], "Full Name");
                        strcpy(formLabels[1], "Specialization");
                        strcpy(formLabels[2], "Contact Number");
                        initForm(3);
                        statusMsg = "Fill in doctor details and click Save.";
                    }
                    if (srchBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_SEARCH_DOCTOR;
                        searchBuf[0] = '\0'; searchLen = 0; searchFocused = true;
                        statusMsg = "Type doctor name or ID to search.";
                    }
                    for (int i = 0; i < doctors.size(); i++) {
                        float ry = 130.f + (i - scrollOffset) * 34.f;
                        if (ry > 90.f && ry < 560.f) {
                            FloatRect row(25.f, ry, 750.f, 32.f);
                            if (row.contains(mp)) {
                                selectedIndex = i;
                                statusMsg = "Selected: " + string(doctors.getAll()[i].getName());
                            }
                        }
                    }
                    if (delBtn.getGlobalBounds().contains(mp)) {
                        if (selectedIndex >= 0 && selectedIndex < doctors.size()) {
                            doctors.removeById(doctors.getAll()[selectedIndex].getId());
                            FileHandler::rewriteDoctors(doctors);
                            statusMsg = "Doctor record deleted.";
                            selectedIndex = -1;
                        }
                        else {
                            statusMsg = "Select a record first to delete.";
                            errorState = true;
                        }
                    }
                }

                // ---- PATIENT LIST ----
                else if (currentState == STATE_PATIENTS) {
                    if (addBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_ADD_PATIENT;
                        strcpy(formLabels[0], "Full Name");
                        strcpy(formLabels[1], "Age");
                        strcpy(formLabels[2], "Contact Number");
                        strcpy(formLabels[3], "Diagnosis / Condition");
                        initForm(4);
                        statusMsg = "Fill in patient details and click Save.";
                    }
                    if (srchBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_SEARCH_PATIENT;
                        searchBuf[0] = '\0'; searchLen = 0; searchFocused = true;
                        statusMsg = "Type patient name or ID to search.";
                    }
                    for (int i = 0; i < patients.size(); i++) {
                        float ry = 130.f + (i - scrollOffset) * 34.f;
                        if (ry > 90.f && ry < 560.f) {
                            FloatRect row(25.f, ry, 750.f, 32.f);
                            if (row.contains(mp)) {
                                selectedIndex = i;
                                statusMsg = "Selected: " + string(patients.getAll()[i].getName());
                            }
                        }
                    }
                    if (delBtn.getGlobalBounds().contains(mp)) {
                        if (selectedIndex >= 0 && selectedIndex < patients.size()) {
                            patients.removeById(patients.getAll()[selectedIndex].getId());
                            FileHandler::rewritePatients(patients);
                            statusMsg = "Patient record deleted.";
                            selectedIndex = -1;
                        }
                        else {
                            statusMsg = "Select a record first to delete.";
                            errorState = true;
                        }
                    }
                }

                // ---- APPOINTMENTS ----
                else if (currentState == STATE_APPOINTMENTS) {
                    if (addBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_ADD_APPOINTMENT;
                        strcpy(formLabels[0], "Patient ID");
                        strcpy(formLabels[1], "Doctor ID");
                        strcpy(formLabels[2], "Date (DD/MM/YYYY)");
                        strcpy(formLabels[3], "Time (HH:MM)");
                        initForm(4);
                        statusMsg = "Fill appointment details and click Save.";
                    }
                    if (delBtn.getGlobalBounds().contains(mp)) {
                        if (selectedIndex >= 0 && selectedIndex < appointments.size()) {
                            appointments.removeById(appointments.getAll()[selectedIndex].getId());
                            statusMsg = "Appointment removed.";
                            selectedIndex = -1;
                        }
                        else {
                            statusMsg = "Select a record first to delete.";
                            errorState = true;
                        }
                    }
                    for (int i = 0; i < appointments.size(); i++) {
                        float ry = 130.f + (i - scrollOffset) * 34.f;
                        if (ry > 90.f && ry < 560.f) {
                            FloatRect row(25.f, ry, 750.f, 32.f);
                            if (row.contains(mp)) {
                                selectedIndex = i;
                                statusMsg = "Appointment on " + string(appointments.getAll()[i].getDate()) + " selected.";
                            }
                        }
                    }
                }

                // ---- BILLING ----
                else if (currentState == STATE_BILLING) {
                    if (addBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_ADD_BILL;
                        strcpy(formLabels[0], "Patient ID");
                        strcpy(formLabels[1], "Amount (Rs.)");
                        strcpy(formLabels[2], "Status (Paid/Pending)");
                        initForm(3);
                        statusMsg = "Fill billing details and click Save.";
                    }
                    if (delBtn.getGlobalBounds().contains(mp)) {
                        if (selectedIndex >= 0 && selectedIndex < bills.size()) {
                            bills.removeById(bills.getAll()[selectedIndex].getId());
                            statusMsg = "Bill record removed.";
                            selectedIndex = -1;
                        }
                        else {
                            statusMsg = "Select a record first to delete.";
                            errorState = true;
                        }
                    }
                    for (int i = 0; i < bills.size(); i++) {
                        float ry = 130.f + (i - scrollOffset) * 34.f;
                        if (ry > 90.f && ry < 560.f) {
                            FloatRect row(25.f, ry, 750.f, 32.f);
                            if (row.contains(mp)) {
                                selectedIndex = i;
                                statusMsg = "Bill #" + to_string(i + 1) + " selected.";
                            }
                        }
                    }
                }

                // ---- ADD DOCTOR FORM ----
                else if (currentState == STATE_ADD_DOCTOR) {
                    for (int i = 0; i < formCount; i++) {
                        float fy = 120.f + i * 70.f;
                        FloatRect fb(80.f, fy, 300.f, 36.f);
                        if (fb.contains(mp)) focusedField = i;
                    }
                    if (saveBtn.getGlobalBounds().contains(mp)) {
                        if (formLengths[0] > 0 && formLengths[1] > 0) {
                            // Explicit Casts Added Here!
                            Doctor d(doctors.size() + 1, formValues[0], formValues[1], formValues[2], "pass", 0.0);
                            doctors.add(d);
                            FileHandler::rewriteDoctors(doctors);
                            currentState = STATE_DOCTORS;
                            statusMsg = "Doctor added successfully.";
                        }
                        else {
                            statusMsg = "ID, Name and Specialization are required.";
                            errorState = true;
                        }
                    }
                    if (cancelBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_DOCTORS;
                        statusMsg = "Add cancelled.";
                    }
                }

                // ---- ADD PATIENT FORM ----
                else if (currentState == STATE_ADD_PATIENT) {
                    for (int i = 0; i < formCount; i++) {
                        float fy = 120.f + i * 70.f;
                        FloatRect fb(80.f, fy, 300.f, 36.f);
                        if (fb.contains(mp)) focusedField = i;
                    }
                    if (saveBtn.getGlobalBounds().contains(mp)) {
                        if (formLengths[0] > 0) {
                            Patient p(patients.size() + 1, formValues[0], atoi(formValues[1]), 'M', formValues[2], "pass", 0.0);
                            patients.add(p);
                            FileHandler::rewritePatients(patients);
                            currentState = STATE_PATIENTS;
                            statusMsg = "Patient added successfully.";
                        }
                        else {
                            statusMsg = "Patient ID and Name are required.";
                            errorState = true;
                        }
                    }
                    if (cancelBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_PATIENTS;
                        statusMsg = "Add cancelled.";
                    }
                }

                // ---- ADD APPOINTMENT FORM ----
                else if (currentState == STATE_ADD_APPOINTMENT) {
                    for (int i = 0; i < formCount; i++) {
                        float fy = 120.f + i * 70.f;
                        FloatRect fb(80.f, fy, 300.f, 36.f);
                        if (fb.contains(mp)) focusedField = i;
                    }
                    if (saveBtn.getGlobalBounds().contains(mp)) {
                        if (formLengths[0] > 0 && formLengths[2] > 0) {
                            // Explicit Casts Added Here!
                            Appointment a(appointments.size() + 1, atoi(formValues[0]), atoi(formValues[1]), formValues[2], formValues[3]);
                            appointments.add(a);
                            currentState = STATE_APPOINTMENTS;
                            statusMsg = "Appointment scheduled.";
                        }
                        else {
                            statusMsg = "Appointment ID, Patient ID and Date are required.";
                            errorState = true;
                        }
                    }
                    if (cancelBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_APPOINTMENTS;
                        statusMsg = "Add cancelled.";
                    }
                }

                // ---- ADD BILL FORM ----
                else if (currentState == STATE_ADD_BILL) {
                    for (int i = 0; i < formCount; i++) {
                        float fy = 120.f + i * 70.f;
                        FloatRect fb(80.f, fy, 300.f, 36.f);
                        if (fb.contains(mp)) focusedField = i;
                    }
                    if (saveBtn.getGlobalBounds().contains(mp)) {
                        if (formLengths[0] > 0 && formLengths[1] > 0 && formLengths[2] > 0) {
                            // Explicit Casts Added Here!
                            Bill b(bills.size() + 1, atoi(formValues[0]), 0, atof(formValues[1]), formValues[2], "N/A");
                            bills.add(b);
                            currentState = STATE_BILLING;
                            statusMsg = "Bill created successfully.";
                        }
                        else {
                            statusMsg = "Bill ID, Patient ID and Amount are required.";
                            errorState = true;
                        }
                    }
                    if (cancelBtn.getGlobalBounds().contains(mp)) {
                        currentState = STATE_BILLING;
                        statusMsg = "Add cancelled.";
                    }
                }

                // ---- SEARCH DOCTOR ----
                else if (currentState == STATE_SEARCH_DOCTOR) {
                    searchFocused = true;
                }

                // ---- SEARCH PATIENT ----
                else if (currentState == STATE_SEARCH_PATIENT) {
                    searchFocused = true;
                }
            }

            // ---- KEYBOARD ----
            if (event.type == Event::TextEntered) {
                // Login screen typing
                if (currentState == STATE_LOGIN) {
                    if (isIdFocused) {
                        if (event.text.unicode == 8 && idLength > 0) idInput[--idLength] = '\0';
                        else if (idLength < 49 && event.text.unicode >= 32 && event.text.unicode <= 126)
                            idInput[idLength++] = (char)event.text.unicode;
                        idInput[idLength] = '\0';
                    }
                    else if (isPassFocused) {
                        if (event.text.unicode == 8 && passLength > 0) {
                            passInput[--passLength] = '\0';
                            passDisplay[passLength] = '\0';
                        }
                        else if (passLength < 49 && event.text.unicode >= 32 && event.text.unicode <= 126) {
                            passInput[passLength] = (char)event.text.unicode;
                            passDisplay[passLength++] = '*';
                        }
                        passInput[passLength] = '\0'; passDisplay[passLength] = '\0';
                    }
                    // Tab switch between id/pass
                    if (event.text.unicode == 9) {
                        isIdFocused = !isIdFocused;
                        isPassFocused = !isPassFocused;
                    }
                }
                // Form screens
                else if (currentState == STATE_ADD_DOCTOR || currentState == STATE_ADD_PATIENT ||
                    currentState == STATE_ADD_APPOINTMENT || currentState == STATE_ADD_BILL) {
                    handleFormText(event.text.unicode);
                }
                // Search screens
                else if (currentState == STATE_SEARCH_DOCTOR || currentState == STATE_SEARCH_PATIENT) {
                    if (event.text.unicode == 8 && searchLen > 0) searchBuf[--searchLen] = '\0';
                    else if (searchLen < 98 && event.text.unicode >= 32 && event.text.unicode <= 126)
                        searchBuf[searchLen++] = (char)event.text.unicode;
                    searchBuf[searchLen] = '\0';
                }
            }

            // Enter key on login
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Return) {
                if (currentState == STATE_LOGIN) {
                    if (Validator::validateAdmin(idInput, passInput, admins)) {
                        currentState = STATE_DASHBOARD;
                        statusMsg = "Login successful.";
                    }
                    else {
                        statusMsg = "Invalid credentials.";
                        errorState = true;
                    }
                }
                else if (currentState == STATE_LOGIN && isIdFocused) {
                    isIdFocused = false; isPassFocused = true;
                }
            }
        }

        // ============================================================
        // DRAWING PHASE
        // ============================================================
        window.clear(BG_DARK);

        // === Background grid pattern (subtle) ===
        for (int gx = 0; gx < 800; gx += 50) {
            RectangleShape gl(Vector2f(1.f, 600.f));
            gl.setPosition((float)gx, 0.f);
            gl.setFillColor(Color(0, 30, 28, 30));
            window.draw(gl);
        }
        for (int gy = 0; gy < 600; gy += 50) {
            RectangleShape gl(Vector2f(800.f, 1.f));
            gl.setPosition(0.f, (float)gy);
            gl.setFillColor(Color(0, 30, 28, 30));
            window.draw(gl);
        }

        // === Top bar ===
        drawPanel(window, 0.f, 0.f, 800.f, 12.f, TEAL_DARK, Color::Transparent, 0.f);

        // ============================================================
        if (currentState == STATE_LOGIN) {
            // ============================================================
                // Title area
            Text med("MediCore", font, 48);
            med.setFillColor(TEAL);
            med.setPosition(260.f, 40.f);
            window.draw(med);
            Text hms("Hospital Management System", font, 16);
            hms.setFillColor(TEXT_GREY);
            hms.setPosition(240.f, 96.f);
            window.draw(hms);

            window.draw(loginCard);

            Text cardTitle("Administrator Login", font, 20);
            cardTitle.setFillColor(CYAN_BRIGHT);
            cardTitle.setPosition(265.f, 148.f);
            window.draw(cardTitle);
            drawDivider(window, 220.f, 178.f, 360.f);

            // ID field
            drawField(window, font, 250.f, 210.f, 300.f, "Admin ID", string(idInput), isIdFocused);

            // Pass field (show passDisplay)
            drawField(window, font, 250.f, 300.f, 300.f, "Password", string(passDisplay), isPassFocused);

            // Tab hint
            Text hint("Tab to switch fields | Enter to login", font, 12);
            hint.setFillColor(TEXT_GREY);
            hint.setPosition(255.f, 350.f);
            window.draw(hint);

            loginBtn.setFillColor(isIdFocused || isPassFocused ? TEAL_DIM : Color(0, 90, 80));
            loginBtn.setOutlineColor(TEAL);
            loginBtn.setOutlineThickness(1.5f);
            drawButton(window, font, loginBtn, "LOGIN", 20);

            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        else if (currentState == STATE_DASHBOARD) {
            // ============================================================
                // Big logo / greeting
            Text logo("MEDICORE", font, 52);
            logo.setFillColor(Color(0, 50, 45, 120));
            logo.setPosition(165.f, 20.f);
            window.draw(logo);
            Text logo2("MEDICORE", font, 52);
            logo2.setFillColor(TEAL);
            logo2.setPosition(163.f, 18.f);
            window.draw(logo2);

            Text sub("Hospital Management System  —  Admin Panel", font, 14);
            sub.setFillColor(TEXT_GREY);
            sub.setPosition(200.f, 80.f);
            window.draw(sub);
            drawDivider(window, 100.f, 106.f, 600.f);

            // Module cards
            string icons[4] = { "Dr.", "Pt.", "Ap.", "Bl." };
            Color cardColors[4] = { Color(0,60,58), Color(0,48,68), Color(48,40,0), Color(48,10,10) };
            Color accentColors[4] = { TEAL, CYAN_BRIGHT, GOLD, RED_ALERT };

            for (int i = 0; i < 4; i++) {
                int col = i % 2, row = i / 2;
                float bx = dashX + col * 280.f;
                float by = dashY + row * 80.f;

                dBtns[i].setFillColor(cardColors[i]);
                dBtns[i].setOutlineColor(accentColors[i]);
                dBtns[i].setOutlineThickness(1.5f);
                window.draw(dBtns[i]);

                // Accent side bar
                RectangleShape sbar(Vector2f(4.f, dashH));
                sbar.setPosition(bx, by);
                sbar.setFillColor(accentColors[i]);
                window.draw(sbar);

                Text icon(icons[i], font, 22);
                icon.setFillColor(accentColors[i]);
                icon.setPosition(bx + 14.f, by + 10.f);
                window.draw(icon);

                Text lbl(dLabels[i], font, 20);
                lbl.setFillColor(TEXT_WHITE);
                lbl.setPosition(bx + 55.f, by + 16.f);
                window.draw(lbl);
            }

            // Stats summary
            drawPanel(window, 100.f, 390.f, 600.f, 68.f, BG_PANEL, TEAL_DARK);
            Text statsTitle("System Overview", font, 14);
            statsTitle.setFillColor(TEXT_GREY);
            statsTitle.setPosition(110.f, 395.f);
            window.draw(statsTitle);

            string statLabels[4] = { "Doctors", "Patients", "Appointments", "Bills" };
            int    statVals[4] = { doctors.size(), patients.size(), appointments.size(), bills.size() };
            for (int i = 0; i < 4; i++) {
                float sx = 115.f + i * 148.f;
                Text sv(to_string(statVals[i]), font, 28);
                sv.setFillColor(TEAL);
                sv.setPosition(sx, 412.f);
                window.draw(sv);
                Text sl(statLabels[i], font, 13);
                sl.setFillColor(TEXT_GREY);
                sl.setPosition(sx, 444.f);
                window.draw(sl);
            }

            logoutBtn.setFillColor(Color(55, 12, 12));
            logoutBtn.setOutlineColor(RED_ALERT);
            logoutBtn.setOutlineThickness(1.f);
            drawButton(window, font, logoutBtn, "LOGOUT", 18);

            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        else if (currentState == STATE_DOCTORS) {
            // ============================================================
            drawTitle(window, font, "Doctor Directory", to_string(doctors.size()) + " doctors on record");
            drawDivider(window, 25.f, 75.f, 750.f);

            addBtn.setFillColor(TEAL_DARK); addBtn.setOutlineColor(TEAL);
            srchBtn.setFillColor(Color(0, 40, 60)); srchBtn.setOutlineColor(CYAN_BRIGHT);
            delBtn.setFillColor(Color(60, 15, 15)); delBtn.setOutlineColor(RED_ALERT);
            drawButton(window, font, addBtn, "+ Add Doctor", 14);
            drawButton(window, font, srchBtn, "Search", 14);
            drawButton(window, font, delBtn, "Delete Selected", 14);

            // Column headers
            drawPanel(window, 25.f, 120.f, 750.f, 26.f, Color(0, 45, 42), TEAL_DARK);
            Text hID("ID", font, 13); hID.setFillColor(TEAL); hID.setPosition(35.f, 124.f); window.draw(hID);
            Text hName("Name", font, 13); hName.setFillColor(TEAL); hName.setPosition(120.f, 124.f); window.draw(hName);
            Text hSpec("Specialization", font, 13); hSpec.setFillColor(TEAL); hSpec.setPosition(310.f, 124.f); window.draw(hSpec);
            Text hCont("Contact", font, 13); hCont.setFillColor(TEAL); hCont.setPosition(560.f, 124.f); window.draw(hCont);

            for (int i = 0; i < doctors.size(); i++) {
                float ry = 148.f + (i - scrollOffset) * 34.f;
                if (ry < 120.f || ry > 555.f) continue;
                Doctor d = doctors.getAll()[i];
                bool sel = (i == selectedIndex);

                Color rowBg = sel ? Color(0, 70, 62) : (i % 2 == 0 ? Color(10, 20, 28) : BG_PANEL);
                drawPanel(window, 25.f, ry, 750.f, 32.f, rowBg, sel ? TEAL : Color(18, 38, 45), sel ? 1.5f : 0.5f);

                auto makeCol = [&](const string& s, float x, Color c) {
                    Text t(s, font, 14); t.setFillColor(c); t.setPosition(x, ry + 8.f); window.draw(t);
                    };

                // Fixed: String Casts & ID Capitalization!
                makeCol(to_string(d.getId()), 35.f, TEXT_GREY);
                makeCol(string(d.getName()), 120.f, TEXT_WHITE);
                makeCol(string(d.getSpecialization()), 310.f, TEAL);
                makeCol(string(d.getContact()), 560.f, TEXT_GREY);
            }
            if (doctors.size() == 0) {
                Text empty("No doctors on record. Click '+ Add Doctor' to begin.", font, 16);
                empty.setFillColor(TEXT_GREY);
                empty.setPosition(180.f, 280.f);
                window.draw(empty);
            }

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        else if (currentState == STATE_PATIENTS) {
            // ============================================================
            drawTitle(window, font, "Patient Records", to_string(patients.size()) + " patients on record");
            drawDivider(window, 25.f, 75.f, 750.f);

            addBtn.setFillColor(TEAL_DARK); addBtn.setOutlineColor(TEAL);
            srchBtn.setFillColor(Color(0, 40, 60)); srchBtn.setOutlineColor(CYAN_BRIGHT);
            delBtn.setFillColor(Color(60, 15, 15)); delBtn.setOutlineColor(RED_ALERT);
            drawButton(window, font, addBtn, "+ Add Patient", 14);
            drawButton(window, font, srchBtn, "Search", 14);
            drawButton(window, font, delBtn, "Delete Selected", 14);

            drawPanel(window, 25.f, 120.f, 750.f, 26.f, Color(0, 45, 42), TEAL_DARK);
            Text hID("ID", font, 13); hID.setFillColor(TEAL); hID.setPosition(35.f, 124.f); window.draw(hID);
            Text hN("Name", font, 13); hN.setFillColor(TEAL); hN.setPosition(120.f, 124.f); window.draw(hN);
            Text hA("Age", font, 13); hA.setFillColor(TEAL); hA.setPosition(310.f, 124.f); window.draw(hA);
            Text hC("Contact", font, 13); hC.setFillColor(TEAL); hC.setPosition(400.f, 124.f); window.draw(hC);
            Text hD("Condition", font, 13); hD.setFillColor(TEAL); hD.setPosition(560.f, 124.f); window.draw(hD);

            for (int i = 0; i < patients.size(); i++) {
                float ry = 148.f + (i - scrollOffset) * 34.f;
                if (ry < 120.f || ry > 555.f) continue;
                Patient p = patients.getAll()[i];
                bool sel = (i == selectedIndex);
                Color rowBg = sel ? Color(0, 70, 62) : (i % 2 == 0 ? Color(10, 20, 28) : BG_PANEL);
                drawPanel(window, 25.f, ry, 750.f, 32.f, rowBg, sel ? TEAL : Color(18, 38, 45), sel ? 1.5f : 0.5f);

                auto col = [&](const string& s, float x, Color c) {
                    Text t(s, font, 14); t.setFillColor(c); t.setPosition(x, ry + 8.f); window.draw(t);
                    };

                // Fixed: String Casts & ID Capitalization!
                col(to_string(p.getId()), 35.f, TEXT_GREY);
                col(string(p.getName()), 120.f, TEXT_WHITE);
                col(to_string(p.getAge()), 310.f, CYAN_BRIGHT);
                col(string(p.getContact()), 400.f, TEXT_GREY);
                col("N/A", 560.f, TEAL); // Removed getDiagnosis as it wasn't in backend
            }
            if (patients.size() == 0) {
                Text empty("No patient records found. Click '+ Add Patient'.", font, 16);
                empty.setFillColor(TEXT_GREY);
                empty.setPosition(180.f, 280.f);
                window.draw(empty);
            }

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        else if (currentState == STATE_APPOINTMENTS) {
            // ============================================================
            drawTitle(window, font, "Appointments", to_string(appointments.size()) + " scheduled");
            drawDivider(window, 25.f, 75.f, 750.f);

            addBtn.setFillColor(TEAL_DARK); addBtn.setOutlineColor(TEAL);
            delBtn.setFillColor(Color(60, 15, 15)); delBtn.setOutlineColor(RED_ALERT);
            drawButton(window, font, addBtn, "+ Schedule", 14);
            drawButton(window, font, delBtn, "Delete Selected", 14);

            drawPanel(window, 25.f, 120.f, 750.f, 26.f, Color(0, 45, 42), TEAL_DARK);
            Text hID("Appt. ID", font, 13); hID.setFillColor(TEAL); hID.setPosition(35.f, 124.f); window.draw(hID);
            Text hPt("Patient ID", font, 13); hPt.setFillColor(TEAL); hPt.setPosition(160.f, 124.f); window.draw(hPt);
            Text hDr("Doctor ID", font, 13); hDr.setFillColor(TEAL); hDr.setPosition(310.f, 124.f); window.draw(hDr);
            Text hDate("Date", font, 13); hDate.setFillColor(TEAL); hDate.setPosition(460.f, 124.f); window.draw(hDate);
            Text hTime("Time", font, 13); hTime.setFillColor(TEAL); hTime.setPosition(610.f, 124.f); window.draw(hTime);

            for (int i = 0; i < appointments.size(); i++) {
                float ry = 148.f + (i - scrollOffset) * 34.f;
                if (ry < 120.f || ry > 555.f) continue;
                Appointment a = appointments.getAll()[i];
                bool sel = (i == selectedIndex);
                Color rowBg = sel ? Color(0, 70, 62) : (i % 2 == 0 ? Color(10, 20, 28) : BG_PANEL);
                drawPanel(window, 25.f, ry, 750.f, 32.f, rowBg, sel ? TEAL : Color(18, 38, 45), sel ? 1.5f : 0.5f);

                auto col = [&](const string& s, float x, Color c) {
                    Text t(s, font, 14); t.setFillColor(c); t.setPosition(x, ry + 8.f); window.draw(t);
                    };

                // Fixed: String Casts, ID Capitalization, and getTimeSlot()!
                col(to_string(a.getId()), 35.f, TEXT_GREY);
                col(to_string(a.getPatientId()), 160.f, TEXT_WHITE);
                col(to_string(a.getDoctorId()), 310.f, CYAN_BRIGHT);
                col(string(a.getDate()), 460.f, TEAL);
                col(string(a.getTimeSlot()), 610.f, TEXT_GREY);
            }
            if (appointments.size() == 0) {
                Text empty("No appointments scheduled. Click '+ Schedule'.", font, 16);
                empty.setFillColor(TEXT_GREY);
                empty.setPosition(175.f, 280.f);
                window.draw(empty);
            }

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        else if (currentState == STATE_BILLING) {
            // ============================================================
            drawTitle(window, font, "Billing & Invoices", to_string(bills.size()) + " records");
            drawDivider(window, 25.f, 75.f, 750.f);

            addBtn.setFillColor(TEAL_DARK); addBtn.setOutlineColor(TEAL);
            delBtn.setFillColor(Color(60, 15, 15)); delBtn.setOutlineColor(RED_ALERT);
            drawButton(window, font, addBtn, "+ New Bill", 14);
            drawButton(window, font, delBtn, "Delete Selected", 14);

            drawPanel(window, 25.f, 120.f, 750.f, 26.f, Color(0, 45, 42), TEAL_DARK);
            Text hBID("Bill ID", font, 13); hBID.setFillColor(TEAL); hBID.setPosition(35.f, 124.f); window.draw(hBID);
            Text hPID("Patient ID", font, 13); hPID.setFillColor(TEAL); hPID.setPosition(180.f, 124.f); window.draw(hPID);
            Text hAmt("Amount (Rs.)", font, 13); hAmt.setFillColor(TEAL); hAmt.setPosition(360.f, 124.f); window.draw(hAmt);
            Text hSt("Status", font, 13); hSt.setFillColor(TEAL); hSt.setPosition(580.f, 124.f); window.draw(hSt);

            double totalPending = 0.0;
            for (int i = 0; i < bills.size(); i++) {
                Bill b = bills.getAll()[i];
                float ry = 148.f + (i - scrollOffset) * 34.f;
                if (ry < 120.f || ry > 555.f) continue;
                bool sel = (i == selectedIndex);

                string status = string(b.getStatus());
                bool isPaid = (status == "Paid" || status == "paid" || status == "PAID");
                if (!isPaid) totalPending += b.getAmount();

                Color rowBg = sel ? Color(0, 70, 62) : (i % 2 == 0 ? Color(10, 20, 28) : BG_PANEL);
                drawPanel(window, 25.f, ry, 750.f, 32.f, rowBg, sel ? TEAL : Color(18, 38, 45), sel ? 1.5f : 0.5f);

                // Fixed: String Casts & ID Capitalization!
                Text tBID(to_string(b.getId()), font, 14);
                tBID.setFillColor(TEXT_GREY); tBID.setPosition(35.f, ry + 8.f); window.draw(tBID);

                Text tPID(to_string(b.getPatientId()), font, 14);
                tPID.setFillColor(TEXT_WHITE); tPID.setPosition(180.f, ry + 8.f); window.draw(tPID);

                ostringstream oss; oss.precision(2); oss << fixed << b.getAmount();
                Text tAmt("Rs. " + oss.str(), font, 14);
                tAmt.setFillColor(GOLD); tAmt.setPosition(360.f, ry + 8.f); window.draw(tAmt);

                Text tSt(status, font, 14);
                tSt.setFillColor(isPaid ? GREEN_OK : RED_ALERT);
                tSt.setPosition(580.f, ry + 8.f); window.draw(tSt);
            }

            if (bills.size() == 0) {
                Text empty("No billing records found. Click '+ New Bill'.", font, 16);
                empty.setFillColor(TEXT_GREY);
                empty.setPosition(185.f, 280.f);
                window.draw(empty);
            }

            // Pending total at bottom
            if (bills.size() > 0) {
                ostringstream oss; oss.precision(2); oss << fixed << totalPending;
                drawPanel(window, 490.f, 530.f, 285.f, 28.f, Color(50, 15, 0), Color(255, 150, 0), 1.f);
                Text tPend("Pending: Rs. " + oss.str(), font, 14);
                tPend.setFillColor(GOLD); tPend.setPosition(500.f, 535.f); window.draw(tPend);
            }

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        // ADD FORMS (shared layout)
        // ============================================================
        else if (currentState == STATE_ADD_DOCTOR || currentState == STATE_ADD_PATIENT ||
            currentState == STATE_ADD_APPOINTMENT || currentState == STATE_ADD_BILL) {

            string formTitle =
                currentState == STATE_ADD_DOCTOR ? "Add New Doctor" :
                currentState == STATE_ADD_PATIENT ? "Add New Patient" :
                currentState == STATE_ADD_APPOINTMENT ? "Schedule Appointment" :
                "Create Bill";
            drawTitle(window, font, formTitle);
            drawDivider(window, 25.f, 75.f, 750.f);

            // Two-column layout: fields on left, help panel on right
            drawPanel(window, 440.f, 90.f, 330.f, 440.f, BG_PANEL, TEAL_DARK);
            Text helpTitle("Quick Help", font, 16);
            helpTitle.setFillColor(TEAL);
            helpTitle.setPosition(455.f, 100.f);
            window.draw(helpTitle);
            drawDivider(window, 455.f, 122.f, 300.f);

            string hints[6];
            if (currentState == STATE_ADD_DOCTOR) {
                hints[0] = "ID: Unique doctor code e.g. D001";
                hints[1] = "Name: Full name of the doctor";
                hints[2] = "Spec.: e.g. Cardiology, ENT";
                hints[3] = "Contact: Phone number";
            }
            else if (currentState == STATE_ADD_PATIENT) {
                hints[0] = "ID: Unique patient code e.g. P001";
                hints[1] = "Name: Full name";
                hints[2] = "Age: Numeric age e.g. 35";
                hints[3] = "Contact: Phone number";
                hints[4] = "Condition: e.g. Diabetes";
            }
            else if (currentState == STATE_ADD_APPOINTMENT) {
                hints[0] = "ID: Unique appt code e.g. A001";
                hints[1] = "Patient ID: Must exist in system";
                hints[2] = "Doctor ID: Must exist in system";
                hints[3] = "Date: DD/MM/YYYY format";
                hints[4] = "Time: HH:MM 24-hr format";
            }
            else {
                hints[0] = "Bill ID: Unique e.g. B001";
                hints[1] = "Patient ID: Must exist in system";
                hints[2] = "Amount: Numeric e.g. 5000.00";
                hints[3] = "Status: Paid or Pending";
            }
            for (int i = 0; i < formCount; i++) {
                Text ht(hints[i], font, 13);
                ht.setFillColor(TEXT_GREY);
                ht.setPosition(458.f, 135.f + i * 24.f);
                window.draw(ht);
            }
            Text tabHint("Tip: Press Tab to jump fields", font, 12);
            tabHint.setFillColor(TEAL_DIM);
            tabHint.setPosition(458.f, 480.f);
            window.draw(tabHint);

            // Draw form fields
            drawForm(window, font, 110.f, 50.f, 360.f);

            saveBtn.setFillColor(TEAL_DARK); saveBtn.setOutlineColor(TEAL);
            cancelBtn.setFillColor(Color(55, 10, 10)); cancelBtn.setOutlineColor(RED_ALERT);
            drawButton(window, font, saveBtn, "SAVE RECORD", 16);
            drawButton(window, font, cancelBtn, "CANCEL", 16);

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        // ============================================================
        // SEARCH SCREENS
        // ============================================================
        else if (currentState == STATE_SEARCH_DOCTOR || currentState == STATE_SEARCH_PATIENT) {
            bool isDrSearch = (currentState == STATE_SEARCH_DOCTOR);
            drawTitle(window, font,
                isDrSearch ? "Search Doctors" : "Search Patients",
                "Type below to filter by name or ID");
            drawDivider(window, 25.f, 75.f, 750.f);

            // Search box
            drawPanel(window, 40.f, 90.f, 500.f, 42.f, Color(8, 22, 28), TEAL, 2.f);
            Text searchPrompt("Search: ", font, 17);
            searchPrompt.setFillColor(TEXT_GREY);
            searchPrompt.setPosition(50.f, 99.f);
            window.draw(searchPrompt);
            Text searchVal(string(searchBuf), font, 17);
            searchVal.setFillColor(CYAN_BRIGHT);
            searchVal.setPosition(130.f, 99.f);
            window.draw(searchVal);
            // Cursor
            RectangleShape sCursor(Vector2f(2.f, 22.f));
            sCursor.setFillColor(TEAL);
            sCursor.setPosition(132.f + searchVal.getGlobalBounds().width, 100.f);
            window.draw(sCursor);

            // Filter and draw results
            int matchCount = 0;
            string query(searchBuf);
            // Convert query to lowercase
            for (int k = 0; k < (int)query.size(); k++)
                if (query[k] >= 'A' && query[k] <= 'Z') query[k] += 32;

            drawPanel(window, 25.f, 144.f, 750.f, 26.f, Color(0, 45, 42), TEAL_DARK);
            Text colH1("ID", font, 13); colH1.setFillColor(TEAL); colH1.setPosition(35.f, 148.f); window.draw(colH1);
            Text colH2("Name", font, 13); colH2.setFillColor(TEAL); colH2.setPosition(160.f, 148.f); window.draw(colH2);
            Text colH3(isDrSearch ? "Specialization" : "Condition", font, 13);
            colH3.setFillColor(TEAL); colH3.setPosition(400.f, 148.f); window.draw(colH3);

            if (isDrSearch) {
                for (int i = 0; i < doctors.size(); i++) {
                    Doctor d = doctors.getAll()[i];
                    string nameLC(d.getName());
                    string idLC = to_string(d.getId());

                    for (int k = 0; k < (int)nameLC.size(); k++) if (nameLC[k] >= 'A' && nameLC[k] <= 'Z') nameLC[k] += 32;
                    for (int k = 0; k < (int)idLC.size(); k++) if (idLC[k] >= 'A' && idLC[k] <= 'Z') idLC[k] += 32;

                    bool match = query.empty() || nameLC.find(query) != string::npos || idLC.find(query) != string::npos;
                    if (!match) continue;

                    float ry = 172.f + matchCount * 34.f;
                    if (ry > 555.f) break;
                    drawPanel(window, 25.f, ry, 750.f, 32.f, matchCount % 2 == 0 ? Color(10, 20, 28) : BG_PANEL, Color(18, 38, 45), 0.5f);

                    auto col = [&](const string& s, float x, Color c) {
                        Text t(s, font, 14); t.setFillColor(c); t.setPosition(x, ry + 8.f); window.draw(t);
                        };
                    col(to_string(d.getId()), 35.f, TEXT_GREY);
                    col(string(d.getName()), 160.f, TEXT_WHITE);
                    col(string(d.getSpecialization()), 400.f, TEAL);
                    matchCount++;
                }
            }
            else {
                for (int i = 0; i < patients.size(); i++) {
                    Patient p = patients.getAll()[i];
                    string nameLC(p.getName());
                    string idLC = to_string(p.getId());

                    for (int k = 0; k < (int)nameLC.size(); k++) if (nameLC[k] >= 'A' && nameLC[k] <= 'Z') nameLC[k] += 32;
                    for (int k = 0; k < (int)idLC.size(); k++) if (idLC[k] >= 'A' && idLC[k] <= 'Z') idLC[k] += 32;

                    bool match = query.empty() || nameLC.find(query) != string::npos || idLC.find(query) != string::npos;
                    if (!match) continue;

                    float ry = 172.f + matchCount * 34.f;
                    if (ry > 555.f) break;
                    drawPanel(window, 25.f, ry, 750.f, 32.f, matchCount % 2 == 0 ? Color(10, 20, 28) : BG_PANEL, Color(18, 38, 45), 0.5f);

                    auto col = [&](const string& s, float x, Color c) {
                        Text t(s, font, 14); t.setFillColor(c); t.setPosition(x, ry + 8.f); window.draw(t);
                        };
                    col(to_string(p.getId()), 35.f, TEXT_GREY);
                    col(string(p.getName()), 160.f, TEXT_WHITE);
                    col("N/A", 400.f, TEAL); // Removed getDiagnosis
                    matchCount++;
                }
            }

            if (matchCount == 0) {
                Text noResult("No matching records found.", font, 16);
                noResult.setFillColor(TEXT_GREY);
                noResult.setPosition(270.f, 300.f);
                window.draw(noResult);
            }
            else {
                Text resCount(to_string(matchCount) + " result(s) found", font, 13);
                resCount.setFillColor(TEAL_DIM);
                resCount.setPosition(560.f, 98.f);
                window.draw(resCount);
            }

            backBtn.setFillColor(Color(15, 35, 45)); backBtn.setOutlineColor(TEAL_DIM);
            drawButton(window, font, backBtn, "< BACK", 14);
            drawStatusBar(window, font, statusMsg, errorState);
        }

        window.display();
    }

    // --- SAVE & EXIT ---
    FileHandler::rewritePatients(patients);
    FileHandler::rewriteDoctors(doctors);
    return 0;
}