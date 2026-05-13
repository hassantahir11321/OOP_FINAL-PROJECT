#ifndef HOSPITAL_EXCEPTION_H
#define HOSPITAL_EXCEPTION_H
#include<cstring>
class HospitalException {
protected:
	char message[200];
public:
	HospitalException(const char* msg) {
		strncpy(message, msg, 199);
		message[199] = '\0';
	}
	virtual const char* what() const {
		return message;
	}

};
class FileNotFoundException : public HospitalException {
public:
	FileNotFoundException() : HospitalException("Error: Required data file could not be opened on startup.") {}
};

class InsufficientFundsException : public HospitalException {
public:
	InsufficientFundsException() : HospitalException("Error: Patient balance is less than the required amount.") {}
};

class InvalidInputException : public HospitalException {
public:
	InvalidInputException() : HospitalException("Error: User input failed validation.") {}
};

class SlotUnavailableException : public HospitalException {
public:
	SlotUnavailableException() : HospitalException("Error: The requested time slot is already occupied.") {}
};

#endif // Ends the include guard