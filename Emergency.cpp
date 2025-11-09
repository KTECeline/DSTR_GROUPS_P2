// ============================================================================
// Emergency.cpp
// Role 3: Emergency Department Management using Priority Queue (Array-based)
// ----------------------------------------------------------------------------
// WHY PRIORITY QUEUE?
// - Realism: Emergency cases differ in urgency; critical patients (priority 1)
//   must be treated first, not just in arrival order.
// - Approach: Implemented using array-based priority queue (sorted by priority).
//   Lower number = higher priority (1 = most critical).
// - Efficiency: Simpler than heap (since max 100 cases); O(n) insertion, O(1)
//   retrieval of top-priority case — fine for hospital scale.
// - Integration: Automatically loads existing emergencies from "emergency.txt"
//   and imports new patients from "patients.txt".
// - Reliability: Input validation, unique ID handling, sorting, and persistence.
// ============================================================================


#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <unordered_set>
#include "Emergency.hpp"
using namespace std;

// ===========================================================
// Constructor — Initialize & Auto-load Data
// ===========================================================
// Loads previously logged emergency cases from "emergency.txt"
// and new patient data from "patients.txt" (if available).
EmergencyDepartment::EmergencyDepartment() {
    size = 0;
    loadExistingEmergencies();
    loadPatientsFromFile();
}

// ===========================================================
// Helper: Convert String to Lowercase
// ===========================================================
// Ensures case-insensitive searches (e.g., "John" == "john").
// O(n) time, where n = string length.
string EmergencyDepartment::toLowerCase(string str) {
    for (std::string::size_type i = 0; i < str.length(); ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] = str[i] + 32;
    }
    return str;
}

// ===========================================================
// Helper: Get Validated Integer Input (Safe Input Handling)
// ===========================================================
// Re-prompts user if input is invalid, non-integer, or out of range.
int EmergencyDepartment::getValidatedInput(int min, int max, string prompt) {
    int choice;
    int attempts = 0;

    while (true) {
        cout << prompt;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[!] Invalid input. Please enter a number.\n";
            attempts++;
        }
        else if (choice < min || choice > max) {
            cout << "[!] Invalid choice. Please enter between " << min << " and " << max << ".\n";
            attempts++;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        }

        if (attempts >= 3) {
            cout << "\n[!] Too many invalid attempts. Returning...\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return -1;
        }
    }
}

// ===========================================================
// Helper: Sort Cases by Priority (Ascending Order)
// ===========================================================
// Simple bubble sort: O(n²) worst case (acceptable for ≤100 records).
void EmergencyDepartment::sortCases() {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (cases[j].priority > cases[j + 1].priority)
                swap(cases[j], cases[j + 1]);
        }
    }
}

// ===========================================================
// Helper: Generate Next Available Unique ID
// ===========================================================
// Reads all IDs from "emergency.txt" to ensure no duplication.
int EmergencyDepartment::generateNextID() {
    unordered_set<int> existingIDs;
    ifstream file("data/emergency.txt");
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr;
        if (getline(ss, idStr, ',')) {
            try {
                existingIDs.insert(stoi(idStr));
            } catch (...) {}
        }
    }
    file.close();

    int newID = 1;
    while (existingIDs.find(newID) != existingIDs.end()) {
        newID++;
    }
    return newID;
}

// ===========================================================
// Load Existing Emergency Cases
// ===========================================================
// Reads "data/emergency.txt" and fills local array. Each line =
// ID, Name, Type, Priority. Then sorts by priority for processing.
void EmergencyDepartment::loadExistingEmergencies() {
    ifstream file("data/emergency.txt");
    if (!file.is_open()) {
        cout << "[!] No existing emergency data found.\n";
        return;
    }

    string line;
    int count = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, type, priorityStr;

        if (getline(ss, idStr, ',') &&
            getline(ss, name, ',') &&
            getline(ss, type, ',') &&
            getline(ss, priorityStr, ',')) {

            EmergencyCase temp;
            temp.patientID = stoi(idStr);
            temp.patientName = name;
            temp.emergencyType = type;
            temp.priority = stoi(priorityStr);

            if (size < 100) {
                cases[size++] = temp;
                count++;
            }
        }
    }
    file.close();
    sortCases();
    cout << "[✓] Loaded " << count << " existing emergency cases.\n";
}

// ===========================================================
// Load Patients from "patients.txt" (Avoid Duplicates)
// ===========================================================
// Reads from patient list, only imports those not already in
// emergency.txt. Default priority = 6 (low urgency).
void EmergencyDepartment::loadPatientsFromFile() {
    ifstream file("data/patients.txt");
    if (!file.is_open()) {
        cout << "[!] patients.txt not found. Skipping new patient import.\n";
        return;
    }

    unordered_set<int> existingIDs;
    for (int i = 0; i < size; i++) {
        existingIDs.insert(cases[i].patientID);
    }

    string line;
    int newCount = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, type;
        if (getline(ss, idStr, ',') && getline(ss, name, ',') && getline(ss, type, ',')) {
            int pid = stoi(idStr);
            if (existingIDs.find(pid) == existingIDs.end()) {
                EmergencyCase temp;
                temp.patientID = pid;
                temp.patientName = name;
                temp.emergencyType = type;
                temp.priority = 6;

                if (size < 100) {
                    cases[size++] = temp;
                    saveCaseToFile(temp);
                    existingIDs.insert(pid);
                    newCount++;
                }
            }
        }
    }

    file.close();
    sortCases();
    cout << "[✓] Added " << newCount << " new unique patients from patients.txt.\n";
}

// ===========================================================
// Save Single Case to File (Append Mode)
// ===========================================================
void EmergencyDepartment::saveCaseToFile(const EmergencyCase& newCase) {
    ofstream file("data/emergency.txt", ios::app);
    if (!file.is_open()) {
        cout << "[!] Could not open emergency.txt for writing.\n";
        return;
    }
    file << newCase.patientID << "," << newCase.patientName << ","
         << newCase.emergencyType << "," << newCase.priority << endl;
    file.close();
}

// ===========================================================
// Log a New Emergency Case (Insert + Sort)
// ===========================================================
// Prompts user for emergency details, assigns auto ID, sets
// priority (1=critical, 10=least). Re-sorts array afterward.
void EmergencyDepartment::logEmergencyCase() {
    if (size >= 100) {
        cout << "\n[!] Maximum case limit reached.\n";
        return;
    }

    EmergencyCase newCase;
    cout << "\n--- Log New Emergency Case ---\n";

    newCase.patientID = generateNextID();
    cout << "[Auto Assigned] Patient ID: " << newCase.patientID << endl;

    cin.ignore();
    cout << "Enter Patient Name: ";
    getline(cin, newCase.patientName);
    if (newCase.patientName.empty()) {
        cout << "[!] Name cannot be empty.\n";
        return;
    }

    // Emergency type selection
    while (true) {
        cout << "\nSelect Type of Emergency:\n";
        cout << "1. Heart Attack\n2. Road Accident\n3. Asthma Attack\n4. Severe Burn\n5. Other\n";
        int typeChoice = getValidatedInput(1, 5, "Enter your choice (1-5): ");
        if (typeChoice == -1) return;

        switch (typeChoice) {
            case 1: newCase.emergencyType = "Heart Attack"; newCase.priority = 1; break;
            case 2: newCase.emergencyType = "Road Accident"; newCase.priority = 2; break;
            case 3: newCase.emergencyType = "Asthma Attack"; newCase.priority = 3; break;
            case 4: newCase.emergencyType = "Severe Burn"; newCase.priority = 4; break;
            case 5:
                cout << "Enter Custom Type: ";
                getline(cin, newCase.emergencyType);
                newCase.priority = getValidatedInput(1, 10, "Enter Priority Level (1=Critical): ");
                if (newCase.priority == -1) return;
                break;
        }
        break;
    }

    // Insert into array by shifting larger priorities right
    int i = size - 1;
    while (i >= 0 && cases[i].priority > newCase.priority) {
        cases[i + 1] = cases[i];
        i--;
    }
    cases[i + 1] = newCase;
    size++;

    saveCaseToFile(newCase);
    cout << "\n[+] Emergency case logged and saved!\n";
}

// ===========================================================
// Process the Most Critical Case (Dequeue Front)
// ===========================================================
// Removes and displays top-priority case (smallest priority value).
void EmergencyDepartment::processCriticalCase() {
    if (size == 0) {
        cout << "\n[!] No emergency cases to process.\n";
        return;
    }

    cout << "\n--- Processing Most Critical Case ---\n";
    cout << "Patient: " << cases[0].patientName
         << " | Type: " << cases[0].emergencyType
         << " | Priority: " << cases[0].priority << endl;

    // Shift all cases forward
    for (int i = 0; i < size - 1; i++)
        cases[i] = cases[i + 1];
    size--;

    cout << "[✓] Case processed and removed.\n";
}

// ===========================================================
// View All Pending Cases
// ===========================================================
// Displays all cases in ascending order of priority.
void EmergencyDepartment::viewPendingCases() {
    if (size == 0) {
        cout << "\n[!] No pending cases.\n";
        return;
    }

    cout << "\n--- Pending Emergency Cases (By Priority) ---\n";
    cout << "-----------------------------------------------------------\n";
    cout << "No. | ID | Priority | Patient Name        | Emergency Type\n";
    cout << "-----------------------------------------------------------\n";

    for (int i = 0; i < size; i++) {
        cout << i + 1 << "   | " << cases[i].patientID
             << "  | " << cases[i].priority
             << "        | " << cases[i].patientName
             << "        | " << cases[i].emergencyType << endl;
    }
    cout << "-----------------------------------------------------------\n";
}

// ===========================================================
// Search Functions (By Name / Type)
// ===========================================================
void EmergencyDepartment::searchByPatientName() {
    if (size == 0) {
        cout << "\n[!] No cases to search.\n";
        return;
    }

    string name;
    cout << "\nEnter Patient Name to Search: ";
    getline(cin, name);
    string lowerName = toLowerCase(name);

    bool found = false;
    for (int i = 0; i < size; i++) {
        if (toLowerCase(cases[i].patientName) == lowerName) {
            cout << "\n[✓] Found Case:\n";
            cout << "ID: " << cases[i].patientID
                 << "\nName: " << cases[i].patientName
                 << "\nType: " << cases[i].emergencyType
                 << "\nPriority: " << cases[i].priority << endl;
            found = true;
        }
    }

    if (!found)
        cout << "[!] No patient found with name: " << name << endl;
}

void EmergencyDepartment::searchByEmergencyType() {
    if (size == 0) {
        cout << "\n[!] No cases to search.\n";
        return;
    }

    string type;
    cout << "\nEnter Emergency Type to Search: ";
    getline(cin, type);
    string lowerType = toLowerCase(type);

    bool found = false;
    cout << "\n--- Matching Cases ---\n";
    for (int i = 0; i < size; i++) {
        if (toLowerCase(cases[i].emergencyType) == lowerType) {
            cout << "Patient: " << cases[i].patientName
                 << " | Priority: " << cases[i].priority << endl;
            found = true;
        }
    }

    if (!found)
        cout << "[!] No cases found for type: " << type << endl;
}

// ===========================================================
// Update Case Priority (Reorder After Update)
// ===========================================================
void EmergencyDepartment::updatePriority() {
    if (size == 0) {
        cout << "\n[!] No cases available to update.\n";
        return;
    }

    viewPendingCases();
    cout << "\nChoose Update Method:\n";
    cout << "1. By Case Number\n2. By Patient Name\n";
    int method = getValidatedInput(1, 2, "Enter your choice (1-2): ");
    if (method == -1) return;

    bool found = false;
    if (method == 1) {
        int num = getValidatedInput(1, size, "Enter Case Number to Update: ");
        if (num == -1) return;

        cout << "Selected: " << cases[num - 1].patientName << endl;
        int newP = getValidatedInput(1, 10, "Enter New Priority (1=Critical): ");
        if (newP == -1) return;
        cases[num - 1].priority = newP;
        found = true;
    } else {
        string name;
        cout << "Enter Patient Name: ";
        getline(cin, name);
        string lowerName = toLowerCase(name);

        for (int i = 0; i < size; i++) {
            if (toLowerCase(cases[i].patientName) == lowerName) {
                cout << "Current Priority: " << cases[i].priority << endl;
                int newP = getValidatedInput(1, 10, "Enter New Priority: ");
                if (newP == -1) return;
                cases[i].priority = newP;
                found = true;
                break;
            }
        }
    }

    if (found) {
        sortCases();
        cout << "[✓] Priority updated and list reordered.\n";
    } else {
        cout << "[!] Case not found.\n";
    }
}

// ===========================================================
// Wrapper Helper (for main)
// ===========================================================
int EmergencyDepartment::askInput(int min, int max, string prompt) {
    return getValidatedInput(min, max, prompt);
}

// ===========================================================
// Member UI — Menu-Driven Interface for EmergencyDepartment
// Replaces a standalone main so this file can be linked with the
// program's central `main.cpp`. This method lets `main.cpp` call
// `EmergencyDepartment::displayMenu()` to enter the emergency menu.
// ===========================================================
void EmergencyDepartment::displayMenu() {
    int choice;

    do {
        cout << "\n==============================================\n";
        cout << "  HOSPITAL EMERGENCY DEPARTMENT SYSTEM\n";
        cout << "==============================================\n";
        cout << "1. Log Emergency Case\n";
        cout << "2. Process Most Critical Case\n";
        cout << "3. View Pending Cases\n";
        cout << "4. Search by Patient Name\n";
        cout << "5. Search by Emergency Type\n";
        cout << "6. Update Case Priority\n";
        cout << "7. Return to Main Menu\n";
        cout << "----------------------------------------------\n";

        choice = askInput(1, 7, "Enter your choice (1-7): ");
        if (choice == -1) continue;

        switch (choice) {
            case 1: logEmergencyCase(); break;
            case 2: processCriticalCase(); break;
            case 3: viewPendingCases(); break;
            case 4: searchByPatientName(); break;
            case 5: searchByEmergencyType(); break;
            case 6: updatePriority(); break;
            case 7: cout << "\nReturning to main menu...\n"; break;
        }

    } while (choice != 7);
}
