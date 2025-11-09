
// Ambulance.cpp
// Implementation for Role 4: Ambulance Dispatcher using a circular singly-linked list.
// Re-Why circular linked list + circular queue semantics?
// - Rotation Efficiency: Rotating the duty roster is O(1) by advancing a single tail pointer
//   (tail = tail->next). This directly models a fair round-robin rotation where the
//   next ambulance simply becomes the new head without moving or copying nodes.
// - Dynamic Size: Linked list allows dynamic number of ambulances without pre-allocating
//   a fixed-capacity array, which fits typical small-to-medium fleet sizes and avoids
//   artificial capacity limits required by fixed-size circular buffers.
// - O(1) Register (append to tail): Adding a new ambulance at the end of the rotation
//   is a constant-time pointer update. Display is O(n) which is acceptable for small n.
// - Simple Memory Management: Compared to arrays, linked nodes require explicit new/delete;
//   we implement a destructor and clear helpers to avoid leaks. This keeps code focused on
//   core data structure concepts (no STL containers) as required by the assignment.
// - Why not priority queue / stack / plain queue? A priority queue addresses urgency (Role 3),
//   stack is LIFO (wrong semantics), and a plain FIFO queue (non-circular) would require
//   dequeue/enqueue for rotation; circular linked list is a natural, minimal-cost fit.

#include "Ambulance.hpp"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <vector>

using namespace std;

struct Ambulance::Node {
	Record data;
	Node* next;
	Node(const Record& r): data(r), next(nullptr) {}
};

Ambulance::Ambulance(): tail(nullptr), nextId(1) {
    loadFromFile(); // Automatically load roster when object is created
}

Ambulance::~Ambulance() {
	clearAll();
}

void Ambulance::clearAll() {
	if (!tail) return;
	// Break the circle to iterate and delete
	Node* head = tail->next;
	tail->next = nullptr;
	Node* cur = head;
	while (cur) {
		Node* tmp = cur->next;
		delete cur;
		cur = tmp;
	}
	tail = nullptr;
}

bool Ambulance::registerAmbulance() {
    // FUNCTIONALITY 1: REGISTER AMBULANCE
    // Purpose: Add new ambulance to active-duty rotation
    // Implementation: 
    // - Collects vehicle registration, driver name, and optional notes
    // - Performs input validation (non-empty fields)
    // - Checks for duplicate vehicle registrations
    // - Creates new node and appends to tail of circular list
    // - Maintains circular structure: new node points to head, tail points to new node
    // - Auto-increments ID and auto-saves to file

	cout << "Enter vehicle registration: ";
	string reg; cin >> ws; getline(cin, reg);
	cout << "Enter driver name: ";
	string driver; getline(cin, driver);
	cout << "Optional notes: ";
	string notes; getline(cin, notes);

	if (reg.empty() || driver.empty()) {
		cout << "Invalid input. Registration aborted." << endl;
		return false;
	}

	// Duplicate check by registration string (case-sensitive here)
	if (tail) {
		Node* cur = tail->next; // head
		do {
			if (cur->data.vehicleReg == reg) {
				cout << "Ambulance with registration '" << reg << "' already registered." << endl;
				return false;
			}
			cur = cur->next;
		} while (cur != tail->next);
	}

	Record r{ nextId++, reg, driver, notes, 0, 0, false }; // Initialize scheduling fields: shiftStart=0, shiftEnd=0, isOnDuty=false
	Node* node = new Node(r);
	if (!tail) {
		node->next = node;
		tail = node;
	} else {
		node->next = tail->next; // head
		tail->next = node;
		tail = node;
	}
	cout << "Registered ambulance ID " << r.id << ": " << r.vehicleReg << " (" << r.driverName << ")" << endl;
	saveToFile(); // Auto-save after registration
	return true;
}

bool Ambulance::rotateShift() {
    // FUNCTIONALITY 2: ROTATE AMBULANCE SHIFT  
    // Purpose: Implement round-robin scheduling for equal duty time
    // Implementation:
    // - O(1) operation: advances tail pointer (tail = tail->next)
    // - This effectively moves the "head" position to next ambulance
    // - Handles edge cases: empty list, single ambulance (no-op)
    // - Maintains circular structure without node copying/moving
    // Data Structure Advantage: Circular list enables O(1) rotation vs O(n) array shifting

	if (!tail) {
		cout << "No ambulances to rotate." << endl;
		return false;
	}
	if (tail->next == tail) {
		cout << "Only one ambulance registered. Rotation is a no-op." << endl;
		return false;
	}
	tail = tail->next; // advance tail so head moves one step
	cout << "Rotation complete. New head is ambulance ID " << tail->next->data.id << "." << endl;
	return true;
}

void Ambulance::displaySchedule() const {
    // CONSOLIDATED DISPLAY: Shows ambulance schedule with all details
    // Purpose: Single comprehensive display supporting multiple views
    // Features:
    // - Shows all ambulances with complete information
    // - Displays rotation order (Head -> Tail) with shift times
    // - Includes on-duty status for easy dispatch decisions
    // - Sortable by time if needed (rotation order shown by default)

	if (!tail) {
		cout << "No ambulances registered." << endl;
		return;
	}

	// Collect ambulances into vector for potential sorting
	vector<Node*> ambulances;
	Node* cur = tail->next;
	do {
		ambulances.push_back(cur);
		cur = cur->next;
	} while (cur != tail->next);

	cout << "\n[ AMBULANCE SCHEDULE & ROTATION STATUS ]" << endl;
	cout << left << setw(6) << "ID" << setw(14) << "Vehicle" << setw(18) << "Driver" 
		 << setw(14) << "Shift" << setw(10) << "On-Duty" << "Notes" << endl;
	cout << string(95, '-') << endl;

	// Display in rotation order (head to tail)
	for (size_t i = 0; i < ambulances.size(); ++i) {
		const Record& r = ambulances[i]->data;
		
		// Display position indicator
		string position;
		if (i == 0) position = "[HEAD] ";
		else if (i == ambulances.size() - 1) position = "[TAIL]";
		else position = "       ";

		string shiftTime;
		if (r.shiftStart == 0 && r.shiftEnd == 0) {
			shiftTime = "Not assigned";
		} else {
			shiftTime = minutesToTime(r.shiftStart) + "-" + minutesToTime(r.shiftEnd);
		}
		string onDutyStr = r.isOnDuty ? "Yes" : "No";
		
		cout << left << setw(6) << r.id << setw(14) << r.vehicleReg << setw(18) << r.driverName 
			 << setw(14) << shiftTime << setw(10) << onDutyStr << position << r.notes << endl;
	}
}

bool Ambulance::removeAmbulance(int id) {
	if (!tail) return false;
	Node* prev = tail;
	Node* cur = tail->next; // head
	do {
		if (cur->data.id == id) {
			if (cur == prev) {
				// single node
				delete cur;
				tail = nullptr;
			} else {
				prev->next = cur->next;
				if (cur == tail) tail = prev; // removed tail
				delete cur;
			}
			cout << "Removed ambulance ID " << id << "." << endl;
			saveToFile(); // Auto-save after removal
			return true;
		}
		prev = cur;
		cur = cur->next;
	} while (cur != tail->next);
	cout << "Ambulance ID " << id << " not found." << endl;
	return false;
}

bool Ambulance::saveToFile(const string& filename) {
    // Create data/ folder if it doesn't exist (simple approach: try to open and assume folder exists)
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open " << filename << " for writing." << endl;
        return false;
    }
    if (!tail) {
        cout << "No ambulances to save." << endl;
        file.close();
        return true;
    }
    // Write header
    file << "ID,Vehicle,Driver,Notes,ShiftStart,ShiftEnd,IsOnDuty\n";
    // Iterate and write all nodes
    Node* cur = tail->next; // head
    do {
        const Record& r = cur->data;
        file << r.id << ","
             << r.vehicleReg << ","
             << r.driverName << ","
             << r.notes << ","
             << r.shiftStart << ","
             << r.shiftEnd << ","
             << (r.isOnDuty ? "1" : "0") << "\n";
        cur = cur->next;
    } while (cur != tail->next);
    file.close();
    cout << "Saved " << filename << " successfully." << endl;
    return true;
}

bool Ambulance::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Warning: File " << filename << " not found. Starting with empty roster." << endl;
        return false;
    }
    clearAll(); // Clear current list
    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        if (line.empty()) continue;
        // Parse CSV: ID,Vehicle,Driver,Notes,ShiftStart,ShiftEnd,IsOnDuty
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        size_t pos5 = line.find(',', pos4 + 1);
        size_t pos6 = line.find(',', pos5 + 1);
        
        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) continue;
        
        int id = stoi(line.substr(0, pos1));
        string vehicle = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string driver = line.substr(pos2 + 1, pos3 - pos2 - 1);
        
        // Handle Notes field (may contain commas or may not exist in old format)
        string notes, shiftStartStr, shiftEndStr, isOnDutyStr;
        int shiftStart = 0, shiftEnd = 0;
        bool isOnDuty = false;
        
        if (pos4 != string::npos) {
            // New format with scheduling fields
            notes = line.substr(pos3 + 1, pos4 - pos3 - 1);
            shiftStartStr = line.substr(pos4 + 1, pos5 - pos4 - 1);
            shiftEndStr = line.substr(pos5 + 1, pos6 - pos5 - 1);
            isOnDutyStr = line.substr(pos6 + 1);
            
            try {
                shiftStart = stoi(shiftStartStr);
                shiftEnd = stoi(shiftEndStr);
                isOnDuty = (stoi(isOnDutyStr) != 0);
            } catch (...) {
                shiftStart = 0;
                shiftEnd = 0;
                isOnDuty = false;
            }
        } else {
            // Old format without scheduling fields
            notes = line.substr(pos3 + 1);
        }
        
        Record r{ id, vehicle, driver, notes, shiftStart, shiftEnd, isOnDuty };
        Node* node = new Node(r);
        if (!tail) {
            node->next = node;
            tail = node;
        } else {
            node->next = tail->next; // head
            tail->next = node;
            tail = node;
        }
        nextId = max(nextId, id + 1); // Update nextId
    }
    file.close();
    cout << "Loaded " << filename << " successfully." << endl;
    return true;
}

// SCHEDULING METHODS

int Ambulance::timeToMinutes(const string& time) {
    // Convert "HH:MM" format to minutes since midnight
    // Example: "08:30" -> 510 minutes
    try {
        size_t colonPos = time.find(':');
        if (colonPos == string::npos) return -1;
        
        int hours = stoi(time.substr(0, colonPos));
        int minutes = stoi(time.substr(colonPos + 1));
        
        if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
            return -1;
        }
        return hours * 60 + minutes;
    } catch (...) {
        return -1;
    }
}

string Ambulance::minutesToTime(int minutes) {
    // Convert minutes since midnight to "HH:MM" format
    // Example: 510 -> "08:30", 1440 -> "24:00" (valid end-of-day time)
    if (minutes < 0 || minutes > 1440) {
        return "INVALID";
    }
    int hours = minutes / 60;
    int mins = minutes % 60;
    ostringstream oss;
    oss << setfill('0') << setw(2) << hours << ":" << setw(2) << mins;
    return oss.str();
}

bool Ambulance::assignShift(int ambulanceId, int shiftStart, int shiftEnd) {
    // SCHEDULING: Assign shift times to an ambulance
    // Parameters: ambulanceId, shiftStart (minutes since midnight), shiftEnd (minutes since midnight)
    // Validation: ensure start < end, both are valid times (0-1440)
    
    if (shiftStart < 0 || shiftEnd <= shiftStart || shiftEnd > 1440) {
        cout << "Invalid shift times. Start must be before end, and within 0-1440 minutes." << endl;
        return false;
    }
    
    if (!tail) {
        cout << "No ambulances registered." << endl;
        return false;
    }
    
    Node* cur = tail->next; // head
    do {
        if (cur->data.id == ambulanceId) {
            cur->data.shiftStart = shiftStart;
            cur->data.shiftEnd = shiftEnd;
            cout << "Assigned shift to ambulance ID " << ambulanceId << ": "
                 << minutesToTime(shiftStart) << " - " << minutesToTime(shiftEnd) << endl;
            saveToFile();
            return true;
        }
        cur = cur->next;
    } while (cur != tail->next);
    
    cout << "Ambulance ID " << ambulanceId << " not found." << endl;
    return false;
}

void Ambulance::updateDutyStatus() {
    // Update on-duty status for all ambulances based on current system time
    time_t now = time(nullptr);
    struct tm* timeinfo = localtime(&now);
    int currentMinutes = timeinfo->tm_hour * 60 + timeinfo->tm_min;
    
    if (!tail) return;
    
    Node* cur = tail->next;
    do {
        // Check if current time falls within shift window
        if (cur->data.shiftStart == 0 && cur->data.shiftEnd == 0) {
            // No shift assigned
            cur->data.isOnDuty = false;
        } else if (cur->data.shiftStart < cur->data.shiftEnd) {
            // Normal case: start < end (e.g., 8:00 AM - 4:00 PM)
            cur->data.isOnDuty = (currentMinutes >= cur->data.shiftStart && currentMinutes < cur->data.shiftEnd);
        } else {
            // Overnight shift: start > end (e.g., 10:00 PM - 6:00 AM) - not typical, kept for robustness
            cur->data.isOnDuty = (currentMinutes >= cur->data.shiftStart || currentMinutes < cur->data.shiftEnd);
        }
        cur = cur->next;
    } while (cur != tail->next);
}

bool Ambulance::isAmbulanceOnDuty(int ambulanceId) const {
    if (!tail) return false;
    
    Node* cur = tail->next;
    do {
        if (cur->data.id == ambulanceId) {
            return cur->data.isOnDuty;
        }
        cur = cur->next;
    } while (cur != tail->next);
    
    return false;
}

void Ambulance::displayOnDutyAmbulances() const {
    // Display all currently on-duty ambulances (filtered view)
    if (!tail) {
        cout << "No ambulances registered." << endl;
        return;
    }
    
    cout << "\n[ CURRENTLY ON-DUTY AMBULANCES ]" << endl;
    cout << left << setw(6) << "ID" << setw(14) << "Vehicle" << setw(18) << "Driver" 
         << setw(14) << "Shift" << "Notes" << endl;
    cout << string(85, '-') << endl;
    
    bool foundAny = false;
    Node* cur = tail->next;
    do {
        const Record& r = cur->data;
        if (r.isOnDuty) {
            string shiftTime = minutesToTime(r.shiftStart) + "-" + minutesToTime(r.shiftEnd);
            cout << left << setw(6) << r.id << setw(14) << r.vehicleReg << setw(18) << r.driverName 
                 << setw(14) << shiftTime << r.notes << endl;
            foundAny = true;
        }
        cur = cur->next;
    } while (cur != tail->next);
    
    if (!foundAny) {
        cout << "No ambulances currently on duty." << endl;
    }
}

void Ambulance::displayScheduleByTime() const {
    // Display ambulances sorted by shift start time (optional filtered view)
    if (!tail) {
        cout << "No ambulances registered." << endl;
        return;
    }
    
    cout << "\n[ AMBULANCE SCHEDULE (Sorted by Shift Time) ]" << endl;
    cout << left << setw(6) << "ID" << setw(14) << "Vehicle" << setw(18) << "Driver" 
         << setw(14) << "Shift" << setw(10) << "On-Duty" << "Notes" << endl;
    cout << string(95, '-') << endl;
    
    // Collect all ambulances into a temporary vector for sorting
    vector<Node*> ambulances;
    Node* cur = tail->next;
    do {
        ambulances.push_back(cur);
        cur = cur->next;
    } while (cur != tail->next);
    
    // Sort by shiftStart time
    sort(ambulances.begin(), ambulances.end(), [](Node* a, Node* b) {
        return a->data.shiftStart < b->data.shiftStart;
    });
    
    // Display sorted ambulances
    for (Node* node : ambulances) {
        const Record& r = node->data;
        string shiftTime;
        if (r.shiftStart == 0 && r.shiftEnd == 0) {
            shiftTime = "Not assigned";
        } else {
            shiftTime = minutesToTime(r.shiftStart) + "-" + minutesToTime(r.shiftEnd);
        }
        string onDutyStr = r.isOnDuty ? "Yes" : "No";
        cout << left << setw(6) << r.id << setw(14) << r.vehicleReg << setw(18) << r.driverName 
             << setw(14) << shiftTime << setw(10) << onDutyStr << r.notes << endl;
    }
}

void Ambulance::displayMenu() {
    int choice;
    do {
        cout << "\n====== AMBULANCE DISPATCH MENU ======\n"
             << "1. Register Ambulance\n"
             << "2. Rotate Ambulance Shift (Fair Rotation)\n"
             << "3. Display Full Schedule & Status\n"
             << "4. Assign Shift Time to Ambulance\n"
             << "5. Update On-Duty Status (Current Time)\n"
             << "6. Remove Ambulance by ID\n"
             << "0. Back to Main Menu\n"
             << "------------------------------------\n"
             << "Enter your choice: ";		cin >> choice;
		if (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			choice = -1;
		}

		switch (choice) {
			case 1:
				cout << "\n[ Registering Ambulance ]" << endl;
				registerAmbulance();
				break;
			case 2:
				cout << "\n[ Rotating Ambulance Shift ]" << endl;
				rotateShift();
				break;
			case 3:
				cout << "\n[ Full Ambulance Schedule ]" << endl;
				updateDutyStatus();
				displaySchedule();
				break;
			case 4: {
				cout << "\n[ Assigning Shift ]" << endl;
				int id;
				string startStr, endStr;
				cout << "Enter ambulance ID: ";
				cin >> id;
				cout << "Enter shift start time (HH:MM, e.g., 08:30): ";
				cin >> startStr;
				cout << "Enter shift end time (HH:MM, e.g., 16:30): ";
				cin >> endStr;
				
				int startMin = timeToMinutes(startStr);
				int endMin = timeToMinutes(endStr);
				
				if (startMin == -1 || endMin == -1) {
					cout << "Invalid time format. Please use HH:MM (24-hour format)." << endl;
				} else {
					assignShift(id, startMin, endMin);
				}
				break;
			}
			case 5: {
				cout << "\n[ Updating Duty Status ]" << endl;
				updateDutyStatus();
				cout << "Duty status updated based on current system time." << endl;
				break;
			}
			case 6: {
				int id;
				cout << "Enter ambulance ID to remove: ";
				cin >> id;
				if (!cin.fail()) removeAmbulance(id);
				else {
					cin.clear();
					cin.ignore(numeric_limits<streamsize>::max(), '\n');
					cout << "Invalid ID input." << endl;
				}
				break;
			}
			case 0:
				cout << "Returning to main menu..." << endl;
				break;
			default:
				cout << "Invalid choice. Please try again." << endl;
		}

	} while (choice != 0);
}
