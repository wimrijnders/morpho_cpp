#ifndef ACTIVATIONRECORD_H
#define ACTIVATIONRECORD_H
#include <cassert>
#include <string>
#include "../common/AnyObject.h"
#include "Environment.h"



/**
 * Currently, instances form linked lists.
 */
class ActivationRecord: public AnyObject {
private:
	int m_return_address{-1};

	// activation record to use when returning from the current function
	ActivationRecord *m_control_link{nullptr};

	Environment m_environment;

	// Where to go to in case of exception
	// Not used yet
	ActivationRecord *m_exception_continuation{nullptr};

	std::string m_function_name;
	std::string m_file_name;
	int    m_line_number{-1};

public:
	ActivationRecord() {
		// Default implicitly deleted, but defaults should be OK for now
	}

	~ActivationRecord() {
		delete m_control_link;
		assert(m_exception_continuation == nullptr);
	}

	void activate(int return_address, ActivationRecord *control_link) {
		m_return_address = return_address;

		assert(m_control_link == nullptr);
		m_control_link = control_link;
	}

	// Return return address and control link by reference
	void  deactivate(int &program_counter, ActivationRecord *&ar) {
		program_counter = m_return_address;
		ar = m_control_link;
		m_control_link = nullptr;
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
