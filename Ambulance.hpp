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
		// Scheduling fields (minutes since midnight: 0-1439)
		int shiftStart; // start time (e.g., 480 = 8:00 AM)
		int shiftEnd;   // end time (e.g., 960 = 4:00 PM)
		bool isOnDuty;  // current duty status
	};

	Ambulance();
	~Ambulance();

	// Register a new ambulance. Returns true on success, false if duplicate or invalid.
	bool registerAmbulance();

	// Rotate the schedule so next ambulance becomes head/takes duty.
	bool rotateShift();

	// Display the current rotation/order starting from current head.
	void displaySchedule() const;

	// SCHEDULING METHODS
	// Assign a shift to an ambulance (start time and end time in minutes since midnight)
	bool assignShift(int ambulanceId, int shiftStart, int shiftEnd);

	// Get current on-duty ambulances
	void displayOnDutyAmbulances() const;

	// Display schedule sorted by shift time
	void displayScheduleByTime() const;

	// Check if ambulance is currently on duty (based on current system time)
	bool isAmbulanceOnDuty(int ambulanceId) const;

	// Update on-duty status for all ambulances based on current time
	void updateDutyStatus();

	// Helper: convert time string (HH:MM) to minutes since midnight
	static int timeToMinutes(const std::string& time);

	// Helper: convert minutes since midnight to time string (HH:MM)
	static std::string minutesToTime(int minutes);

	// Persist ambulances to file
	bool saveToFile(const std::string& filename = "data/ambulances.txt");

	// Load ambulances from file (clears current list first)
	bool loadFromFile(const std::string& filename = "data/ambulances.txt");

	// Menu driven interface (mirrors PatientAdmission style)
	void displayMenu();

	// Optional: remove ambulance by id
	bool removeAmbulance(int id);
	// Hide implementation details in .cpp
	struct Node;
	Node* tail; // nullptr when empty; tail->next is head
	int nextId;
	// Helper to free list
	void clearAll();
};

#endif // AMBULANCE_HPP

