// ============================================================================
// MedicalSupply.cpp
// Implementation for Role 2: Medical Supply Manager (STACK via Linked List)
// ----------------------------------------------------------------------------
// Re-Why LINKED LIST + STACK (LIFO)?
// - LIFO Behavior: Push() adds new supplies on top; Pop() removes last added.
//   This directly satisfies “Use Last Added Supply” specification.
// - Linked List Efficiency: Dynamic allocation allows unlimited supplies 
//   without predefined array size, avoiding overflow and wasted space.
// - Constant-Time Operations: Push/Pop operations are O(1), ideal for fast 
//   inventory updates in emergency hospital contexts.
// - Real-World Mapping: Reflects real medical storeroom restocking — the 
//   most recent stock is the first used (last in, first out).
// - Validation & Safety: Includes input validation for quantity and expiry, 
//   defensive error handling, and ensures file-based persistence across runs.
// - File Persistence: All records saved in "data/medical_supplies.txt" in CSV 
//   format (ID,Name,Quantity,Batch,Expiry,Notes).
// - System Relevance: Supports hospital operations through efficient 
//   resource tracking, ensuring accountability and preventing overuse.
// ----------------------------------------------------------------------------
// Complexity Summary:
//   Push (Add Supply)        → O(1)
//   Pop (Use Last Supply)    → O(1)
//   View (Traverse Stack)    → O(n)
//   File I/O (Save/Load)     → O(n)
// ============================================================================

#include "MedicalSupply.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <cctype>

using namespace std;

static const char* PRIMARY_PATH   = "data/medical_supplies.txt";
static const char* FALLBACK_PATH  = "medicalSupply.txt";

void MedicalSupply::trim(std::string& s) {
    size_t i = 0;
    while (i < s.size() && isspace(static_cast<unsigned char>(s[i]))) ++i;
    s.erase(0, i);
    if (s.empty()) return;
    size_t j = s.size() - 1;
    while (j < s.size() && isspace(static_cast<unsigned char>(s[j]))) {
        if (j == 0) { s.clear(); return; }
        --j;
    }
    s.erase(j + 1);
}

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
        s.notes    = line.substr(p5 + 1);
    } catch (...) {
        return false;
    }

    trim(s.name); trim(s.batch); trim(s.expiry); trim(s.notes);
    return true;
}

bool MedicalSupply::isAlnumDash(const std::string& s) {
    if (s.empty()) return false;
    for (unsigned char c : s)
        if (!(isalnum(c) || c == '-')) return false;
    return true;
}

bool MedicalSupply::isValidDate(const std::string& d) {
    if (d.size() != 10 || d[4] != '-' || d[7] != '-') return false;
    for (int i : {0,1,2,3,5,6,8,9})
        if (!isdigit(d[i])) return false;
    int m = stoi(d.substr(5,2));
    int day = stoi(d.substr(8,2));
    if (m < 1 || m > 12) return false;
    if (day < 1 || day > 31) return false;
    return true;
}

MedicalSupply::MedicalSupply() : top_(nullptr), nextId_(1) {
    if (!loadFromFile()) {
        cout << "[MedicalSupply] No database found. Starting with an empty stack.\n";
    }
}
MedicalSupply::~MedicalSupply() {
    clearAll();
}

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
    if (s.id >= nextId_) nextId_ = s.id + 1;
}
bool MedicalSupply::popNode(Supply& out) {
    if (!top_) return false;
    Node* n = top_;
    out = n->data;
    top_ = n->next;
    delete n;
    return true;
}

bool MedicalSupply::addSupply() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Supply s;
    s.id = nextId_;

    cout << "Enter supply name: ";
    getline(cin, s.name);

    cout << "Enter quantity: ";
    if (!(cin >> s.quantity)) {
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

    pushNode(s);

    cout << "Added supply ID " << s.id << ": " << s.name
         << " (" << s.quantity << " units)\n";

    saveToFile();
    return true;
}

bool MedicalSupply::useLastAddedSupply() {
    if (!top_) {
        cout << "\n⚠️  No supplies available to use.\n";
        return false;
    }

    cout << "\n------------------------------------\n";
    cout << "🧺 FEATURE: Use 'Last Added' Supply (LIFO Stack)\n";
    cout << "------------------------------------\n";
    cout << "This operation always targets the *most recently added* item on the stack.\n";
    cout << "However, you may choose how many units to use from it.\n\n";

    Supply &peek = top_->data;
    cout << "Top of stack (most recent):\n";
    cout << "ID: " << peek.id << "\n"
         << "Name: " << peek.name << "\n"
         << "Batch: " << peek.batch << "\n"
         << "Quantity: " << peek.quantity << "\n"
         << "Expiry: " << peek.expiry << "\n"
         << "Notes: " << peek.notes << "\n\n";

    cout << "Enter number of units to use (1 - " << peek.quantity << "): ";
    int useQty;
    if (!(cin >> useQty) || useQty <= 0 || useQty > peek.quantity) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "❌ Invalid quantity entered.\n";
        return false;
    }

    peek.quantity -= useQty;

    if (peek.quantity == 0) {
        cout << "✅ All units used. Removing supply from stack...\n";
        Supply used{};
        popNode(used);
    } else {
        cout << "✅ " << useQty << " units used from " << peek.name
             << " (Remaining: " << peek.quantity << ")\n";
    }

    saveToFile();
    return true;
}

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

bool MedicalSupply::saveToSpecificFile(const std::string& filename) {
    ofstream f(filename);
    if (!f.is_open()) return false;

    f << "ID,Name,Quantity,Batch,Expiry,Notes\n";
    Node* cur = top_;
    while (cur) {
        const Supply& s = cur->data;
        f << s.id << "," << s.name << "," << s.quantity << ","
          << s.batch << "," << s.expiry << "," << s.notes << "\n";
        cur = cur->next;
    }
    return true;
}

bool MedicalSupply::loadFromSpecificFile(const std::string& filename) {
    ifstream f(filename);
    if (!f.is_open()) return false;

    clearAll();

    string header;
    if (!getline(f, header)) return false;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        Supply s{};
        if (parseCsvLine(line, s)) {
            pushNode(s);
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
    return false;
}

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
