#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "Instruction.h"
#include "Interpreter.h"

namespace operation {

class StoreArgVal: public Instruction {
private:
	int m_val{0};
	int m_pos2{0};

public:
	StoreArgVal(int ar, int pos, int val) :
		Instruction(ar, pos),
		m_val(val)
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, new IntObject(m_val));
	}
};

class StoreArgVar: public Instruction {
private:
	int m_lev2{0};
	int m_pos2{0};

public:
	StoreArgVar(int ar, int pos, int lev2, int pos2) :
		Instruction(ar, pos),
		m_lev2(lev2),
		m_pos2(pos2)
	{}


	/**
	 * Assume level 0 if not specified
	 */
	StoreArgVar(int ar, int pos, int pos2) :
		StoreArgVar(ar, pos, 0, pos2) 
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *from_ar = interpreter.control_link(m_lev2);
		AnyObject *from_var = from_ar->get(m_pos2);
		assert(from_var != nullptr);

		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, from_var->clone());
	}
};


class StoreArgAcc: public Instruction {

public:
	StoreArgAcc(int ar, int pos) :
		Instruction(ar, pos)
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, interpreter.get_acc());
	}
};

class Return: public Instruction {
private:
	int m_lev{0};

public:
	// For the time being, only control level 0
	Return() :
		Instruction(0,0),
		m_lev(0)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.pop_ar(m_lev);
	}
};

using lib_func = void (Interpreter &);

class Call: public Instruction {
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	// Init with library function
	Call(lib_func &func, int ar) : 
		Instruction(ar, 0),
		m_func(func)
	{}

	// Init with index into the current instruction array
	Call(int position, int ar) : 
		Instruction(ar, 0),
		m_position(position)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.push_ar(m_ar);

		if (m_func != nullptr) {
			assert(m_position == -1);
			m_func(interpreter);
		} else {
			assert(m_position != -1);
			interpreter.jump_absolute(m_position);
		}	
	}
};


class Become: public Instruction {
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	// Init with library function
	Become(lib_func &func, int ar) : 
		Instruction(ar, 0),
		m_func(func)
	{}

	// Init with index into the current instruction array
	Become(int position, int ar) : 
		Instruction(ar, 0),
		m_position(position)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.push_ar(m_ar, true);	// Apparently the only difference with Call

		if (m_func != nullptr) {
			assert(m_position == -1);
			m_func(interpreter);
		} else {
			assert(m_position != -1);
			interpreter.jump_absolute(m_pos);
		}	
	}
};


class GoFalse: public Instruction {
private:
	int m_target;

public:
	GoFalse(int target) : 
		Instruction(0, 0),
		m_target(target)
	{
	}

	void execute(Interpreter &interpreter) override {
		//TODO: test acc for false

		interpreter.jump_relative(m_target);
	}
};


class MakeVal: public Instruction {
private:
	int m_val;		// int only for now

public:
	MakeVal(int val) : 
		Instruction(0, 0),
		m_val(val)
	{
	}

	void execute(Interpreter &interpreter) override {
		//TODO: test acc for false

		interpreter.set_acc(new IntObject(m_val));
	}
};


} // namespace operation


template<typename T>
T *get_convert(ActivationRecord *ar, int index) {
	assert(ar != nullptr);

	AnyObject *var = ar->get(index);
	assert(var != nullptr);
	T *ret = dynamic_cast<T *>(var);
	assert(ret != nullptr);

	return ret;
}

#endif // OPERATIONS_H
