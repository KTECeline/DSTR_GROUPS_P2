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
    PatientAdmission pa;
    MedicalSupply* ms = nullptr;   // create later when needed
    Emergency* ed = nullptr;
    Ambulance* ad = nullptr;

    int choice;

    do {
        cout << "\n==============================================\n";
        cout << "      HOSPITAL PATIENT CARE MANAGEMENT SYSTEM\n";
        cout << "==============================================\n";
        cout << "1. Patient Admission (Role 1: Array + FIFO Queue)\n";
        cout << "2. Medical Supply Management (Role 2)\n";
        cout << "3. Emergency Department (Role 3)\n";
        cout << "4. Ambulance Dispatch (Role 4)\n";
        cout << "0. Exit\n";
        cout << "----------------------------------------------\n";
        cout << "Enter choice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                pa.displayMenu();
                break;

            case 2:
                cout << "\n[ Medical Supply Management not implemented yet ]\n";
                // Later: ms->displayMenu();
                break;

            case 3:
                cout << "\n[ Emergency Department not implemented yet ]\n";
                // Later: ed->displayMenu();
                break;

            case 4:
                cout << "\n[ Ambulance Dispatch not implemented yet ]\n";
                // Later: ad->displayMenu();
                break;

            case 0:
                cout << "\nExiting system... Goodbye.\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}