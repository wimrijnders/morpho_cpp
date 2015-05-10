#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "Variables.h"

class Environment {
private:

	Variables m_variables;

	// Environment are chained, from direct enclosing to top-level global.
	Environment *m_enclosing_environment{nullptr};

public:
	Environment() {
		// Default implicitly deleted, but defaults should be OK for now
		
		assert(false); //TODO: Check this
	}

	AnyObject *get(int index) {
		return m_variables.get(index);
	}

	void set(int index, AnyObject *obj) {
		return m_variables.set(index, obj);
	}
};



#endif // ENVIRONMENT_H
