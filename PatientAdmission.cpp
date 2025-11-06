// PatientAdmission.cpp
// Implementation for Role 1: Manual FIFO queue via array.
// Re-Why ARRAY + FIFO QUEUE?
// - FIFO Behavior: Enqueue (rear++) adds new patients to end; dequeue (front++) removes oldest—preserves arrival order.
//   Directly solves "Admit Patient" (add), "Discharge Patient" (remove earliest), "View" (show in order).
// - Array Efficiency: Contiguous storage = fast access (no pointer chasing like linked lists); O(1) push/pop.
//   Bounded size prevents memory leaks; linear scan for view/search is O(n) but trivial (n<=100, hospital-scale).
// - Relevance to System: Supports "patient queues" challenge without priorities (Role 3 uses priority queue for urgency).
//   Performance: Constant time core ops = handles peak flows; aligns with "efficient management" in outbreaks.
// - Vs. Alternatives: Not Stack (LIFO—would discharge newest first, unfair); not Circular (no rotation needed here, Role 4).
//   Array > Linked List: Simpler (no new/delete), faster for fixed max—focus on core DS, not mem mgmt.
// Validation/Edges: Input checks, error msgs—boosts code quality (readability, correctness).
// Uppercase: Standardizes output (e.g., "john" → "JOHN") for clean reports.

#include "PatientAdmission.hpp"
#include <iostream>
#include <limits>
#include <iomanip>
#include <cctype>  // For toupper (uppercase transform).
using namespace std;

// Helper: Uppercase a string (innovation: Standardizes names for clean records/search).
void toUppercase(string& str) {
    for (char& c : str) {
        c = toupper(static_cast<unsigned char>(c));  // Safe toupper.
    }
    // Why? Uniform display (e.g., "John" → "JOHN"); O(m) time, m=length (negligible).
}

PatientAdmission::PatientAdmission() : front(0), rear(0), currentSize(0), nextId(1) {
    // Reset for clean start. Why? Simulates new shift.
}

bool PatientAdmission::admitPatient() {
    // Enqueue: Prompt, auto-ID, uppercase name, add if not full.
    if (isFull()) {
        cout << "Queue full! Max " << MAX_PATIENTS << " patients." << endl;
        return false;
    }
    int id = nextId++;  // Auto-increment.
    string name, condition;
    cout << "Patient Name: ";
    cin >> ws;  // Clear buffer.
    getline(cin, name);
    cout << "Condition: ";
    getline(cin, condition);
    if (name.empty() || condition.empty()) {
        cout << "Invalid input." << endl;
        nextId--;  // Undo ID.
        return false;
    }
    toUppercase(name);  // Transform to caps.
    queue[rear++] = {id, name, condition};
    currentSize++;
    cout << "Admitted: " << name << " (ID " << id << ")." << endl;
    return true;
}

bool PatientAdmission::dischargePatient() {
    // Dequeue: Remove/display front if not empty.
    if (isEmpty()) {
        cout << "Queue empty." << endl;
        return false;
    }
    Patient p = queue[front++];
    cout << "Discharged: " << p.name << " (ID " << p.id << ", " << p.condition << ")." << endl;
    currentSize--;
    return true;
}

void PatientAdmission::viewPatientQueue() const {
    // Display: Linear from front (O(n) scan—simple for report). Names already caps.
    if (isEmpty()) {
        cout << "Queue empty." << endl;
        return;
    }
    cout << "\n[ Patient Queue (Earliest First) ]:" << endl;
    cout << left << setw(5) << "ID" << setw(15) << "Name" << "Condition" << endl;
    cout << "-----------------------" << string(15, '-') << endl;
    for (int i = front; i < front + currentSize; ++i) {  // Linear access.
        int idx = i < MAX_PATIENTS ? i : i % MAX_PATIENTS;  // Safe if overflow.
        const Patient& p = queue[idx];
        cout << setw(5) << p.id << setw(15) << p.name << p.condition << endl;
    }
    cout << "Total: " << currentSize << "/" << MAX_PATIENTS << endl;
}

bool PatientAdmission::searchPatientById(int searchId) const {
    // Bonus: Scan queue for ID (O(n); from front for order relevance).
    // Why? Efficient check without full view (e.g., "Is patient X waiting?").
    if (searchId < 1) {
        cout << "Invalid ID." << endl;
        return false;
    }
    for (int i = front; i < front + currentSize; ++i) {
        int idx = i < MAX_PATIENTS ? i : i % MAX_PATIENTS;
        if (queue[idx].id == searchId) {
            const Patient& p = queue[idx];
            cout << "Found: " << p.name << " (ID " << p.id << ", " << p.condition << ") at position " << (i - front + 1) << "." << endl;
            return true;
        }
    }
    cout << "ID " << searchId << " not in queue." << endl;
    return false;
}

void PatientAdmission::displayMenu() {
    int choice;

    do {
        cout << "\n====== Patient Admission Menu ======\n"
             << "1. Admit Patient\n"
             << "2. Discharge Patient\n"
             << "3. View Patient Queue\n"
             << "4. Search Patient by ID\n"
             << "0. Exit Program\n"
             << "-----------------------------------\n"
             << "Enter your choice: ";

        cin >> choice;

        // Handle input error (non-integer)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:
                cout << "\n"  << "[ Admitting Patient ]"  << endl;  
                admitPatient();
                break;

            case 2:
                cout << "\n"  << "[ Discharging Patient ]"  << endl;  
                dischargePatient();
                break;

            case 3:
                cout << "\n"  << "[ Viewing Patient Queue ]"  << endl;
                viewPatientQueue();
                break;

            case 4: {
                int id;
                cout << "Enter Patient ID to search: ";
                cin >> id;
                if (!cin.fail()) {
                    searchPatientById(id);
                } else {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid ID input.\n";
                }
                break;
            }

            case 0:
                cout << "Exiting program...\n";
                break;

            default:
                cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);
}
