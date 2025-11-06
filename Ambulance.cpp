
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
#include <iomanip>
#include <limits>

using namespace std;

struct Ambulance::Node {
	Record data;
	Node* next;
	Node(const Record& r): data(r), next(nullptr) {}
};

Ambulance::Ambulance(): tail(nullptr), nextId(1) {}

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
	cout << "Enter vehicle registration (no spaces): ";
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

	Record r{ nextId++, reg, driver, notes };
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
	return true;
}

bool Ambulance::rotateShift() {
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
	if (!tail) {
		cout << "No ambulances registered." << endl;
		return;
	}
	cout << "\n[ Ambulance Schedule (Head -> ... -> Tail) ]" << endl;
	cout << left << setw(6) << "ID" << setw(15) << "Vehicle" << setw(20) << "Driver" << "Notes" << endl;
	cout << string(60, '-') << endl;
	Node* cur = tail->next; // head
	do {
		const Record& r = cur->data;
		cout << left << setw(6) << r.id << setw(15) << r.vehicleReg << setw(20) << r.driverName << r.notes << endl;
		cur = cur->next;
	} while (cur != tail->next);
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
			return true;
		}
		prev = cur;
		cur = cur->next;
	} while (cur != tail->next);
	cout << "Ambulance ID " << id << " not found." << endl;
	return false;
}

void Ambulance::displayMenu() {
	int choice;
	do {
		cout << "\n====== Ambulance Dispatch Menu ======\n"
			 << "1. Register Ambulance\n"
			 << "2. Rotate Ambulance Shift\n"
			 << "3. Display Ambulance Schedule\n"
			 << "4. Remove Ambulance by ID\n"
			 << "0. Back to Main Menu\n"
			 << "------------------------------------\n"
			 << "Enter your choice: ";

		cin >> choice;
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
				cout << "\n[ Displaying Ambulance Schedule ]" << endl;
				displaySchedule();
				break;
			case 4: {
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
