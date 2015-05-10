#ifndef VARIABLES_H
#define VARIABLES_H
#include <vector>
#include "AnyObject.h"


class Variables {
private:
	// Convention in VM: temp have negative indexes, local >= 0
	std::vector<AnyObject *> m_temp_variables;
	std::vector<AnyObject *> m_local_variables;

public:
	~Variables() {
		for (auto &ptr: m_temp_variables) {
			delete ptr;
			ptr = nullptr;
		}

		for (auto &ptr: m_local_variables) {
			delete ptr;
			ptr = nullptr;
		}
	}


	// Index values < 0 are temp variables
	// Index values >= 0 are local variables

	// accessing value from non-existent location returns null
	AnyObject *get(int index) {
		if (index < 0) {
			index = -index;

			if (index < m_temp_variables.size()) {
				return m_temp_variables[index];
			}	
		} else {
			if (index < m_local_variables.size()) {
				return m_local_variables[index];
			}	
		}

		return nullptr;
	}


	// Storing value in non-existent location creates it
	void set(int index, AnyObject *obj) {
		if (index < 0) {
			index = -index;

			m_temp_variables[index] = obj;
		} else {
			m_local_variables[index] = obj;
		}
	}

};

#endif // VARIABLES_H
