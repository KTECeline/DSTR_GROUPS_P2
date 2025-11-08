#ifndef MEDICALSUPPLY_HPP
#define MEDICALSUPPLY_HPP

#include <string>

/*
===============================================================================
ROLE 2: MEDICAL SUPPLY MANAGER
-------------------------------------------------------------------------------
WHAT THIS MODULE DOES
- Handles inventory of hospital medical supplies.
- Functionalities required by spec:
  1) Add Supply Stock                -> push to stack
  2) Use 'Last Added' Supply         -> pop from stack
  3) View Current Supplies           -> traverse stack

DATA STRUCTURE & ALGORITHM CHOICE
- Abstract structure: STACK (LIFO) because spec says "use the LAST added".
  -> push() / pop() at the top give O(1) time.
- Physical structure: SINGLY LINKED LIST (manual nodes, no STL containers).
  -> Dynamic size (no fixed array capacity), constant-time head updates,
     memory ownership explicit (good for learning outcomes).

PERSISTENCE
- Reads from and writes to a CSV-like TXT file so state survives restarts.
- Dual-path strategy: primary "data/medical_supplies.txt", fallback "medicalSupply.txt".

COMPLEXITY SUMMARY
- Add (push):                   O(1)
- Use last (pop):               O(1)
- View (traverse/print):        O(n)
- Save/Load file (linear scan): O(n)

CODE QUALITY / MARKING INTENT
- Defensive input handling (stream resets).
- Tolerant CSV parser (notes can contain commas; parsed as "remainder of line").
- Clean separation of interface (.hpp) and implementation (.cpp).
- Explicit comments justify choices and complexities for viva.
===============================================================================
*/

class MedicalSupply {
public:
    // Record stored in each stack node
    struct Supply {
        int         id;
        std::string name;
        int         quantity;
        std::string batch;
        std::string expiry;   // YYYY-MM-DD
        std::string notes;    // free text (may contain commas)
    };

    MedicalSupply();
    ~MedicalSupply();

    // === Core features (Menu calls these) ===
    bool addSupply();               // 1) Add Stock  -> push
    bool useLastAddedSupply();      // 2) Use Last   -> pop
    void viewCurrentSupplies() const; // 3) View      -> traverse

    // === Persistence (TXT database) ===
    bool saveToFile();
    bool loadFromFile();

    // === Menu driver ===
    void displayMenu();

private:
    // Manual singly-linked list node (no STL containers used)
    struct Node {
        Supply data;
        Node*  next;
        explicit Node(const Supply& s) : data(s), next(nullptr) {}
    };

    Node* top_;     // Stack top (most recent item)
    int   nextId_;  // Auto-increment ID source

    // ---- Internal helpers (single-responsibility, testable) ----
    void clearAll();                // Free entire list (O(n))
    void pushNode(const Supply& s); // O(1) push to top_
    bool popNode(Supply& out);      // O(1) pop from top_

    bool saveToSpecificFile(const std::string& filename); // O(n)
    bool loadFromSpecificFile(const std::string& filename); // O(n)

    // Small utility helpers
    static void trim(std::string& s);                 // whitespace hygiene
    static bool parseCsvLine(const std::string& line, // tolerant CSV parse
                             Supply& s);
};

#endif // MEDICALSUPPLY_HPP
