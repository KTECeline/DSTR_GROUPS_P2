// PatientAdmission.hpp
// Header for Role 1: Patient Admission Clerk.
// Data Structure Choice: ARRAY + FIFO QUEUE
// Why QUEUE (FIFO principle)?
// - FIFO (First-In-First-Out) perfectly matches role requirements: Patients are admitted in arrival order (enqueue at rear)
//   and discharged in the same order (dequeue from front—"remove the earliest admitted patient after treatment").
// - Ensures fairness in routine flows: No line-jumping (unlike priority queue for emergencies in Role 3).
// - "View Patient Queue" displays waiting list in exact order (front to rear), simulating hospital waiting room.
// - Aligns with scenario: "Managing patient queues" during peak ops—efficient for sequential processing.
// Why ARRAY as underlying structure?
// - Fixed size (MAX_PATIENTS=100): Predictable memory (O(1) access), suits bounded hospital queue (e.g., limited beds).
//   No dynamic resizing needed—avoids complexity/overhead of linked lists (pointers, fragmentation).
// - O(1) enqueue/dequeue amortized (rear++/front++); simple linear traversal for view (O(n), fine for n=100).
// - Vs. Linked List: Array faster (contiguous memory, cache-friendly); linked list better for unbounded but adds nodes (unneeded here).
// - No STL (<queue>/<vector>): Manual impl per rules—core C++ only.
// Innovation: Auto-ID (prevents dupes), uppercase names (uniform records), search bonus (quick lookup for efficiency).
// Challenges Addressed: "Routine patient flows" (FIFO order) + "user-friendly menu-driven" (sub-menu integration).

#ifndef PATIENT_ADMISSION_HPP
#define PATIENT_ADMISSION_HPP

#include <string>

struct Patient {
    int id;          // Auto-generated unique ID
    std::string name;  // Uppercase for uniformity
    std::string condition;
    // Why struct? Bundles patient data cleanly for queue ops.
};

class PatientAdmission {
private:
    static const int MAX_PATIENTS = 100;  // Why? Predictable size; overflow handled gracefully.
    Patient queue[MAX_PATIENTS];          // Array storage.
    int front;                            // Earliest patient index.
    int rear;                             // Next add index.
    int currentSize;                      // Count for quick checks.
    int nextId;                           // Auto-ID starter (innovation: avoids manual dupes).

public:
    PatientAdmission();                   // Init empty queue.
    ~PatientAdmission() {}                // No cleanup needed.

    // Core 3 functionalities:
    bool admitPatient();                  // Add to rear (prompts input, auto-ID, uppercase name).
    bool dischargePatient();              // Remove from front, display.
    void viewPatientQueue() const;        // Show FIFO order.

    // Bonus (innovation):
    bool searchPatientById(int id) const; // O(n) scan: Find/display by ID.

    // Helpers:
    bool isEmpty() const { return currentSize == 0; }
    bool isFull() const { return currentSize == MAX_PATIENTS; }
    int getQueueSize() const { return currentSize; }

    // Menu for demo/integration:
    void displayMenu();                   // Sub-menu loop (now with option 4: search).
};

#endif