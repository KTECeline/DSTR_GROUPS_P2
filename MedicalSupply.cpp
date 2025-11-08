#include "MedicalSupply.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <cctype>   // isspace

using namespace std;

// -----------------------------------------------------------------------------
// Persistence strategy: Prefer project "data/" folder, fallback to local file.
// This makes the module resilient across different teammate setups.
// -----------------------------------------------------------------------------
static const char* PRIMARY_PATH   = "data/medical_supplies.txt";
static const char* FALLBACK_PATH  = "medicalSupply.txt";

// ============================================================================
// Utility: trim()  ---  whitespace hygiene around user/file inputs
// - Keeps printed tables cleaner and avoids subtle CSV spacing bugs.
// - Cost: O(m) for string length m (negligible vs I/O).
// ============================================================================
void MedicalSupply::trim(std::string& s) {
    // left trim
    size_t i = 0;
    while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
    s.erase(0, i);
    // right trim
    if (s.empty()) return;
    size_t j = s.size() - 1;
    while (j < s.size() && isspace(static_cast<unsigned char>(s[j]))) {
        if (j == 0) { s.clear(); return; }
        --j;
    }
    s.erase(j + 1);
}

// ============================================================================
// Utility: parseCsvLine()
// CSV format we write/read:
//   ID,Name,Quantity,Batch,Expiry,Notes...
// Technique:
// - We find the first 5 commas explicitly (fixed fields).
// - "Notes" is the remainder of the line so it may contain commas safely.
// Robustness:
// - try/catch around stoi conversions; trim for cleanliness.
// ============================================================================
bool MedicalSupply::parseCsvLine(const std::string& line, Supply& s) {
    size_t p1 = line.find(',');
    if (p1 == string::npos) return false;
    size_t p2 = line.find(',', p1 + 1);
    if (p2 == string::npos) return false;
    size_t p3 = line.find(',', p2 + 1);
    if (p3 == string::npos) return false;
    size_t p4 = line.find(',', p3 + 1);
    if (p4 == string::npos) return false;
    size_t p5 = line.find(',', p4 + 1);
    if (p5 == string::npos) return false;

    try {
        s.id       = stoi(line.substr(0, p1));
        s.name     = line.substr(p1 + 1, p2 - p1 - 1);
        s.quantity = stoi(line.substr(p2 + 1, p3 - p2 - 1));
        s.batch    = line.substr(p3 + 1, p4 - p3 - 1);
        s.expiry   = line.substr(p4 + 1, p5 - p4 - 1);
        s.notes    = line.substr(p5 + 1); // remainder (can include commas)
    } catch (...) {
        return false; // invalid numeric field etc.
    }

    trim(s.name); trim(s.batch); trim(s.expiry); trim(s.notes);
    return true;
}

// ============================================================================
// Constructor/Destructor
// - On construction, attempt to load the database (primary then fallback).
// - On destruction, free the linked list to avoid leaks.
// ============================================================================
MedicalSupply::MedicalSupply() : top_(nullptr), nextId_(1) {
    if (!loadFromFile()) {
        cout << "[MedicalSupply] No database found. Starting with an empty stack.\n";
    }
}
MedicalSupply::~MedicalSupply() {
    clearAll();
}

// ============================================================================
// Memory management helpers
// clearAll(): O(n) delete of every node (safe cleanup).
// pushNode(): O(1) stack push at head.
// popNode():  O(1) stack pop from head.
// ----------------------------------------------------------------------------
// Design note:
// - We centralize ID progression in pushNode() to prevent double increments.
//   If a loaded record has ID >= current nextId_, bump nextId_ accordingly.
// ============================================================================
void MedicalSupply::clearAll() {
    while (top_) {
        Node* t = top_;
        top_ = top_->next;
        delete t;
    }
}
void MedicalSupply::pushNode(const Supply& s) {
    Node* n = new Node(s);
    n->next = top_;
    top_ = n;
    if (s.id >= nextId_) nextId_ = s.id + 1; // keep nextId_ monotonic
}
bool MedicalSupply::popNode(Supply& out) {
    if (!top_) return false;
    Node* n = top_;
    out = n->data;
    top_ = n->next;
    delete n;
    return true;
}

// ============================================================================
// FEATURE 1: addSupply()  ---  push (O(1))
// Technique highlights:
// - Defensive input handling: clear stream, validate quantity numeric.
// - Trimming ensures clean, consistent stored values.
// - Assign ID from nextId_, push, then save (persistence after every change).
// ============================================================================
bool MedicalSupply::addSupply() {
    // clear stray newline before getline usage
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Supply s;
    s.id = nextId_; // allocated here; pushNode() will maintain nextId_ invariant

    cout << "Enter supply name: ";
    getline(cin, s.name);

    cout << "Enter quantity: ";
    if (!(cin >> s.quantity)) {            // validate numeric input
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid quantity.\n";
        return false;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    cout << "Enter batch number: ";
    getline(cin, s.batch);
    cout << "Enter expiry date (YYYY-MM-DD): ";
    getline(cin, s.expiry);
    cout << "Notes/Remarks: ";
    getline(cin, s.notes);

    trim(s.name); trim(s.batch); trim(s.expiry); trim(s.notes);
    if (s.name.empty() || s.quantity <= 0) {
        cout << "Invalid input. Supply not added.\n";
        return false;
    }

    pushNode(s); // O(1)

    cout << "Added supply ID " << s.id << ": " << s.name
         << " (" << s.quantity << " units)\n";

    // DO NOT ++nextId_ here (pushNode keeps it correct).
    saveToFile(); // persist after mutation
    return true;
}

// ============================================================================
// FEATURE 2: useLastAddedSupply()  ---  pop (O(1))
// Technique highlights:
// - LIFO proof point: returns the most recently added item.
// - Graceful handling on empty stack.
// - Persist immediately to keep file snapshot in-sync.
// ============================================================================
bool MedicalSupply::useLastAddedSupply() {
    if (!top_) {
        cout << "No supplies available.\n";
        return false;
    }
    Supply used{};
    popNode(used); // O(1)

    cout << "\nUsing last added supply:\n"
         << "ID: "      << used.id       << "\n"
         << "Name: "    << used.name     << "\n"
         << "Batch: "   << used.batch    << "\n"
         << "Quantity: "<< used.quantity << "\n"
         << "Expiry: "  << used.expiry   << "\n"
         << "Notes: "   << used.notes    << "\n";

    saveToFile(); // persist after mutation
    return true;
}

// ============================================================================
// FEATURE 3: viewCurrentSupplies()  ---  traverse (O(n))
// Technique highlights:
// - Prints from TOP -> BOTTOM to reflect actual stack order.
// - Clear, column-aligned output for readability in demos.
// ============================================================================
void MedicalSupply::viewCurrentSupplies() const {
    if (!top_) {
        cout << "No supplies in stock.\n";
        return;
    }

    cout << "\n[ Current Medical Supplies (Top → Bottom) ]\n";
    cout << left << setw(6)  << "ID"
                 << setw(20) << "Name"
                 << setw(10) << "Qty"
                 << setw(12) << "Batch"
                 << setw(15) << "Expiry"
                 << "Notes\n";
    cout << string(80, '-') << "\n";

    Node* cur = top_;
    while (cur) {
        const Supply& s = cur->data;
        cout << left << setw(6)  << s.id
                     << setw(20) << s.name
                     << setw(10) << s.quantity
                     << setw(12) << s.batch
                     << setw(15) << s.expiry
                     << s.notes << "\n";
        cur = cur->next;
    }
}

// ============================================================================
// Persistence: save/load (O(n))
// Design:
// - Write header + each node from TOP → BOTTOM (matches current view).
// - On load, push each parsed record so the last line becomes the TOP,
//   preserving the same LIFO semantics after restart.
// Robustness:
// - Dual path; informative messages; tolerant parser.
// ============================================================================
bool MedicalSupply::saveToSpecificFile(const std::string& filename) {
    ofstream f(filename);
    if (!f.is_open()) return false;

    f << "ID,Name,Quantity,Batch,Expiry,Notes\n";
    Node* cur = top_;
    while (cur) {
        const Supply& s = cur->data;
        // Notes may contain commas; we serialize as-is because our parser
        // treats the remainder of line as Notes on read.
        f << s.id << "," << s.name << "," << s.quantity << ","
          << s.batch << "," << s.expiry << "," << s.notes << "\n";
        cur = cur->next;
    }
    return true;
}

bool MedicalSupply::loadFromSpecificFile(const std::string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return false;

    clearAll(); // reset in-memory stack

    string header;
    if (!getline(f, header)) return false; // skip header safely

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        Supply s{};
        if (parseCsvLine(line, s)) {
            pushNode(s); // push so that last line ends up at top (correct LIFO)
        }
    }
    return true;
}

bool MedicalSupply::saveToFile() {
    if (saveToSpecificFile(PRIMARY_PATH)) {
        cout << "[MedicalSupply] Saved to " << PRIMARY_PATH << "\n";
        return true;
    }
    if (saveToSpecificFile(FALLBACK_PATH)) {
        cout << "[MedicalSupply] Saved to " << FALLBACK_PATH << " (fallback)\n";
        return true;
    }
    cout << "[MedicalSupply] ERROR: failed to save database.\n";
    return false;
}

bool MedicalSupply::loadFromFile() {
    if (loadFromSpecificFile(PRIMARY_PATH)) {
        cout << "[MedicalSupply] Loaded from " << PRIMARY_PATH << "\n";
        return true;
    }
    if (loadFromSpecificFile(FALLBACK_PATH)) {
        cout << "[MedicalSupply] Loaded from " << FALLBACK_PATH << "\n";
        return true;
    }
    return false; // caller prints friendly message
}

// ============================================================================
// Menu driver (simple CLI)
// - Validates numeric choice and loops until user returns to main menu.
// - Each choice maps directly to the 3 required functionalities.
// ============================================================================
void MedicalSupply::displayMenu() {
    int choice;
    do {
        cout << "\n====== Medical Supply Manager ======\n"
             << "1. Add Supply Stock\n"
             << "2. Use 'Last Added' Supply\n"
             << "3. View Current Supplies\n"
             << "0. Back to Main Menu\n"
             << "------------------------------------\n"
             << "Enter your choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:
                cout << "\n[ Adding New Supply ]\n";
                addSupply();
                break;
            case 2:
                cout << "\n[ Using Last Added Supply ]\n";
                useLastAddedSupply();
                break;
            case 3:
                cout << "\n[ Viewing Current Supplies ]\n";
                viewCurrentSupplies();
                break;
            case 0:
                cout << "Returning to main menu...\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);
}
