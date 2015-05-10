#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <vector>

class Interpreter;

class Instruction {
protected:
	int m_ar{0};
	int m_pos{0};

public:
		Instruction(int ar, int pos) :
			m_ar(ar),
			m_pos(pos) 
		{}

	virtual ~Instruction() {}
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
			ptr = nullptr;		// Paranoia
		}
	}
};

#endif // INSTRUCTION_H
