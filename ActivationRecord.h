#ifndef ACTIVATIONRECORD_H
#define ACTIVATIONRECORD_H
#include "AnyObject.h"
#include "Environment.h"


/**
 * Currently, instances form linked lists.
 */
class ActivationRecord: public AnyObject {
private:
	int m_return_address;

	// activation record to use when returning from the current function
	ActivationRecord *m_control_link{nullptr};

	Environment m_environment;

	// Where to go to in case of exception
	// Not clear yet how this should work
	AnyObject *m_exception_continuation{nullptr};

	string m_function_name;
	string m_file_name;
	int    m_line_number;

public:
	ActivationRecord() {
		// Default implicitly deleted, but defaults should be OK for now
		
		assert(false); //TODO: Check this
	}

	~ActivationRecord() {
		delete m_control_link;
	}

	AnyObject *clone() override {
		assert(false);	// Don't let this happen (for now)
		return nullptr;
	}

	int return_address() { return m_return_address; }


	AnyObject *get(int index) {
		return m_environment.get(index);
	}

	void set(int index, AnyObject *obj) {
		return m_environment.set(index, obj);
	}

	ActivationRecord *control_link() { return m_control_link; }
};

#endif // ACTIVATIONRECORD_H
