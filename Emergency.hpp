// Emergency.hpp
// Header for Role 3: Emergency Department.
// Data Structure Choice: ARRAY + PRIORITY QUEUE
// 
// Why PRIORITY QUEUE (based on severity ranking)?
// - Emergencies must be handled by *criticality*, not arrival order.
// - Lower priority number = higher urgency (e.g., 1 = critical, 5 = mild).
// - Patients are sorted automatically — ensures most severe cases processed first.
// - When processing: highest-priority case dequeued first (simulating triage system).
//
// Why ARRAY as underlying structure?
// - Fixed size (MAX_CASES = 100) fits manageable emergency list.
// - Simpler than dynamic/linked lists (faster cache access, minimal memory overhead).
// - Sorting on insert (O(n)) is acceptable for small n=100.
// - Predictable, efficient, and easily maintained for educational projects.
//
// Innovation:
// - Auto-ID generation (no duplicate patient IDs).
// - Auto-loading from patient records (avoids re-entry).
// - File I/O sync with "emergency.txt" for persistence.
//
// Challenges Addressed:
// - Triage fairness (priority-based order)
// - Smooth integration with patient module (shared data files)
// - Safe user input validation

#ifndef EMERGENCY_HPP
#define EMERGENCY_HPP

#include <string>
#include <unordered_set>

// ------------------------------------------------------------
// STRUCT: EmergencyCase — Represents one emergency patient record
// ------------------------------------------------------------
struct EmergencyCase {
    int patientID;              // Unique ID (auto-generated)
    std::string patientName;    // Patient name
    std::string emergencyType;  // Emergency category (Heart Attack, etc.)
    int priority;               // 1 = most critical, higher = less urgent
};

// ------------------------------------------------------------
// CLASS: EmergencyDepartment — Manages all emergency cases
// ------------------------------------------------------------
class EmergencyDepartment {
private:
    static const int MAX_CASES = 100;     // Max array capacity
    EmergencyCase cases[MAX_CASES];       // Array of cases
    int size;                             // Current case count

    // === Helper Functions ===
    std::string toLowerCase(std::string str);                // Convert string to lowercase
    int getValidatedInput(int min, int max, std::string prompt); // Validate safe integer input
    void sortCases();                                        // Sort by priority (ascending)
    void saveCaseToFile(const EmergencyCase& newCase);       // Save one record to file
    void loadPatientsFromFile();                             // Load new patients (from patients.txt)
    void loadExistingEmergencies();                          // Load existing emergency cases (from emergency.txt)
    int generateNextID();                                    // Generate next unique ID

public:
    EmergencyDepartment();       // Constructor — auto-loads data
    ~EmergencyDepartment() {}    // Destructor (no dynamic memory)

    // === Core Functionalities ===
    void logEmergencyCase();     // Add new emergency record
    void processCriticalCase();  // Handle & remove most urgent case
    void viewPendingCases();     // Display all pending emergencies
    void searchByPatientName();  // Search by patient name (case-insensitive)
    void searchByEmergencyType();// Search by type (e.g., “Heart Attack”)
    void updatePriority();       // Update existing patient priority

    // === UI/Integration ===
    void displayMenu();          // Sub-menu for Emergency Department
    int askInput(int min, int max, std::string prompt); // Input wrapper
};

#endif
