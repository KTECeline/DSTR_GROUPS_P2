// PatientAdmission.cpp
// Implementation for Role 1: Manual FIFO queue via array.
// Re-Why ARRAY + FIFO QUEUE?
// - FIFO Behavior: Enqueue (rear++) adds new patients to end; dequeue (front++) removes oldest—preserves arrival order.
//   Directly solves "Admit Patient" (add), "Discharge Patient" (remove earliest), "View" (show in order).
// - Array Efficiency: Contiguous storage = fast access (no pointer chasing like linked lists); O(1) push/pop.
//   Bounded size prevents memory leaks; linear scan for view/search is O(n) but trivial (n<=100, hospital-scale).
// - Relevance to System: Supports "patient queues" challenge without priorities (Role 3 uses priority queue for urgency).
//   Performance: Constant time core ops = handles peak flows; aligns with "efficient management" in outbreaks.
//   Array > Linked List: Simpler (no new/delete), faster for fixed max—focus on core DS, not mem mgmt.
//   Validation/Edges: Input checks, error msgs—boosts code quality (readability, correctness).
//.  Innovation: Auto-ID (prevents dupes), uppercase names (uniform records), search bonus (quick lookup for efficiency).
//  Saved patients to file for persistence across runs, in simple CSV format (ID,Name,Condition) in "data/patients.txt". 

#include "PatientAdmission.hpp"
#include <iostream>
#include <iomanip>
#include <cctype>  // For toupper (uppercase transform).
#include <fstream>
#include <sstream>
#include <ctime>    // For time()
using namespace std;

// Helper: Uppercase a string (innovation: Standardizes names for clean records/search).
void toUppercase(string& str) {
    for (char& c : str) {
        c = toupper(static_cast<unsigned char>(c));  // Safe toupper.
    }
    // Why? Uniform display (e.g., "John" → "JOHN"); O(m) time, m=length (negligible).
}

PatientAdmission::PatientAdmission() : front(0), rear(0), currentSize(0), nextId(1) {
    // Load existing patients from file on startup
    loadPatientsFromFile("data/patients.txt");
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
    toUppercase(name);     // Transform name to caps
    toUppercase(condition); // Transform condition to caps
    queue[rear++] = {id, name, condition};
    currentSize++;
    
    // Print hospital admission ticket
    cout << "\n╔═════════════════════════════════════════╗" << endl;
    cout << "║         HOSPITAL ADMISSION TICKET         ║" << endl;
    cout << "╠═════════════════════════════════════════╣" << endl;
    cout << "║ Patient ID: " << setw(30) << left << id << "║" << endl;
    cout << "║ Name: " << setw(34) << left << name << "║" << endl;
    cout << "║ Condition: " << setw(30) << left << condition << "║" << endl;
    time_t now = time(nullptr);
    string datetime = ctime(&now);
    datetime = datetime.substr(0, datetime.length()-1);  // Remove newline
    cout << "║ Time: " << setw(34) << left << datetime << "║" << endl;
    cout << "╚═════════════════════════════════════════╝" << endl;
    
    savePatientsToFile("data/patients.txt");  // Save after admission
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
    savePatientsToFile("data/patients.txt");  // Save after discharge
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
    // Scan queue for ID (O(n); from front for order relevance).
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

bool PatientAdmission::loadPatientsFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cout << "Note: No existing patient file found. Starting fresh." << endl;
        return false;
    }

    string line;
    int maxId = 0;
    while (getline(file, line)) {
        stringstream ss(line);
        string idStr, name, condition;
        
        // Parse CSV format (ID,Name,Condition)
        if (getline(ss, idStr, ',') && 
            getline(ss, name, ',') && 
            getline(ss, condition)) {
            
            int id = stoi(idStr);
            maxId = max(maxId, id);  // Track highest ID
            
            if (currentSize < MAX_PATIENTS) {
                queue[rear].id = id;
                queue[rear].name = name;
                queue[rear].condition = condition;
                rear++;
                currentSize++;
            }
        }
    }
    
    nextId = maxId + 1;  // Set next ID to be one more than highest loaded
    file.close();
    return true;
}

bool PatientAdmission::savePatientsToFile(const string& filename) const {
    ofstream file(filename);
    if (!file) {
        cout << "Error: Cannot open file for saving." << endl;
        return false;
    }

    for (int i = front; i < front + currentSize; ++i) {
        int idx = i < MAX_PATIENTS ? i : i % MAX_PATIENTS;
        const Patient& p = queue[idx];
        file << p.id << "," << p.name << "," << p.condition << endl;
    }

    file.close();
    return true;
}
