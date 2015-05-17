#ifndef OLD_OPERATIONS_H
#define OLD_OPERATIONS_H
#include "../common/Operation.h"
#include "Interpreter.h"
#include "old_support.h"






namespace operation {

//////////////////////////////
// Morpho2 operations
//////////////////////////////

using lib_func = void (Interpreter &);

/**
 * @brief Perform a call to either a built-in function or
 *        a position in the current operations array.
 *
 */
class Call: public Operation {
	int m_ar{0};
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	/**
	 * @brief Init with library function
	 */
	Call(lib_func &func, int ar) :
		m_ar(ar),
		m_func(func)
	{}


	/**
	 * @brief Init with index into the current instruction array
	 *
	 * The position is passed as a relative jump from the current
	 * program counter.
	 */
	Call(int position, int ar) :
		m_ar(ar),
		m_position(position)
	{}


	void execute(Interpreter &interpreter) override {
		interpreter.call_init(m_ar);

		if (m_func != nullptr) {
			assert(m_position == -1);
			m_func(interpreter);
		} else {
			assert(m_position != -1);
			interpreter.jump_absolute(m_position);
		}
	}
};


class Return: public Operation {
private:
	int m_lev{0};

public:
	// For the time being, only control level 0
	Return() :
		m_lev(0)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.ret(m_lev);
	}
};


class MakeVal: public Operation {
private:
	int m_val;		// int only for now

public:
	MakeVal(int val) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		//TODO: test acc for false

		interpreter.set_acc(new IntObject(m_val));
	}
};




//////////////////////////////
// Old operations
//////////////////////////////

class StoreArgVal: public Operation {
private:
	int m_ar{0};
	int m_pos{0};
	int m_val{0};
	int m_pos2{0};

public:
	StoreArgVal(int ar, int pos, int val) :
		m_ar(ar),
		m_pos(pos),
		m_val(val)
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, new IntObject(m_val));
	}
};

class StoreArgVar: public Operation {
private:
	int m_ar{0};
	int m_pos{0};
	int m_lev2{0};
	int m_pos2{0};

public:
	StoreArgVar(int ar, int pos, int lev2, int pos2) :
		m_ar(ar),
		m_pos(pos),
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


class StoreArgAcc: public Operation {
private:
	int m_ar{0};
	int m_pos{0};

public:
	StoreArgAcc(int ar, int pos) :
		m_ar(ar),
		m_pos(pos)
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, interpreter.get_acc()->clone());
	}
};


class Become: public Operation {
private:
	int m_ar{0};
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	// Init with library function
	Become(lib_func &func, int ar) : 
		m_ar(ar),
		m_func(func)
	{}

	// Init with index into the current instruction array
	Become(int position, int ar) : 
		m_ar(ar),
		m_position(position)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.become(m_ar);

		if (m_func != nullptr) {
			assert(m_position == -1);
			m_func(interpreter);
		} else {
			assert(m_position != -1);
			interpreter.jump_absolute(m_position);
		}	
	}
};


class GoFalse: public Operation {
private:
	int m_target;

public:
	GoFalse(int target) : 
		m_target(target)
	{
	}

	void execute(Interpreter &interpreter) override {
		// Test acc for false
		AnyObject *var = interpreter.get_acc();
		assert(var != nullptr);
		BoolObject *ret = dynamic_cast<BoolObject *>(var);
		assert(ret != nullptr);

		if (ret->val() == false) {
			interpreter.jump_relative(m_target);
		}
	}
};


} // namespace operation



#endif // OLD_OPERATIONS_H
