#include <iostream>
#include "PatientAdmission.hpp"  // Include all role headers
#include "MedicalSupply.hpp"
#include "Emergency.hpp"
#include "Ambulance.hpp"
using namespace std;

class MedicalSupply;
class Emergency;
class Ambulance;
class PatientAdmission;

int main() {
    // Initialize role objects
    PatientAdmission pa;
    MedicalSupply* ms = nullptr;
    Emergency* ed = nullptr;
    Ambulance* ad = nullptr;

    int choice;
    cout << "=== Hospital Patient Care Management System ===" << endl;
    cout << "1. Patient Admission (Role 1)" << endl;
    cout << "2. Medical Supply Management (Role 2)" << endl;
    cout << "3. Emergency Department (Role 3)" << endl;
    cout << "4. Ambulance Dispatch (Role 4)" << endl;
    cout << "0. Exit" << endl;
    cout << "Enter choice: ";
    cin >> choice;

    // Role-based menu routing (expand with sub-menus for each role's 3 functionalities)
    switch (choice) {
        case 1:
            // Sub-menu for Role 1: Admit, Discharge, View
            pa.displayMenu();  // Example: Call a menu method in your class
            break;
        // ... similar for cases 2-4
        case 0: return 0;
        default: cout << "Invalid choice!" << endl;
    }
    // Loop for continuous menu (while true, or until exit)
    return 0;
}