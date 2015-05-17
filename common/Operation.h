#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <vector>

class Interpreter;

class Operation {
public:
	virtual ~Operation() {}
	virtual void execute(Interpreter &interpreter) = 0;
};


class OperationArray : public std::vector<Operation *> {
public:
	OperationArray(Operation *data[]) {
		Operation **ptr = data;

		while (*ptr != nullptr) {
			push_back(*ptr);
			++ptr;
		}
	}


	~OperationArray() {
		for (auto &ptr: *this) {
			delete ptr;
			ptr = nullptr;		// Paranoia
		}
	}
};

#endif // INSTRUCTION_H
