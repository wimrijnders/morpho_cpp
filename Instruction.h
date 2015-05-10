#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <vector>

class Interpreter;

class Instruction {
public:
	virtual void execute(Interpreter &interpreter) = 0;
};


class InstructionArray : public std::vector<Instruction *> {
public:
	InstructionArray(Instruction *data[]) {
		Instruction **ptr = data;

		while (*ptr != nullptr) {
			push_back(*ptr);
			++ptr;
		}
	}


	~InstructionArray() {
		for (auto &ptr: *this) {
			delete ptr;
			ptr = nullptr;
		}
	}
};

#endif // INSTRUCTION_H
