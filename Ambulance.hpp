// Ambulance.hpp
#ifndef AMBULANCE_HPP
#define AMBULANCE_HPP

#include <string>

class Ambulance {
public:
	struct Record {
		int id;
		std::string vehicleReg;
		std::string driverName;
		std::string notes;
	};

	Ambulance();
	~Ambulance();

	// Register a new ambulance. Returns true on success, false if duplicate or invalid.
	bool registerAmbulance();

	// Rotate the schedule so next ambulance becomes head/takes duty.
	bool rotateShift();

	// Display the current rotation/order starting from current head.
	void displaySchedule() const;

	// Optional: remove ambulance by id
	bool removeAmbulance(int id);

	// Persist ambulances to file
	bool saveToFile(const std::string& filename = "data/ambulances.txt");

	// Load ambulances from file (clears current list first)
	bool loadFromFile(const std::string& filename = "data/ambulances.txt");

	// Menu driven interface (mirrors PatientAdmission style)
	void displayMenu();

private:
	// Hide implementation details in .cpp
	struct Node;
	Node* tail; // nullptr when empty; tail->next is head
	int nextId;
	// Helper to free list
	void clearAll();
};

#endif // AMBULANCE_HPP

