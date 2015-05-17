#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "../common/Operation.h"
#include "../Interpreter.h"


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
	int m_nargs{0};
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	/**
	 * @brief Init with library function
	 */
	Call(lib_func &func, int nargs) :
		m_nargs(nargs),
		m_func(func)
	{}


	/**
	 * @brief Init with index into the current instruction array
	 *
	 * The position is passed as a relative jump from the current
	 * program counter.
	 */
	Call(int position, int nargs) :
		m_nargs(nargs),
		m_position(position)
	{}


	void execute(Interpreter &interpreter) override {
		assert(false);  // TODO
		//interpreter.call_init(m_ar);

		if (m_func != nullptr) {
			assert(m_position == -1);
			m_func(interpreter);
		} else {
			assert(m_position != -1);
			interpreter.jump_absolute(m_position);
		}
	}
};

class CallR: public Operation {
private:
	int m_ar{0};
	lib_func *m_func{nullptr};
	int m_position{-1};

public:
	// Init with library function
	CallR(lib_func &func, int ar) :
		m_ar(ar),
		m_func(func)
	{}

	// Init with index into the current instruction array
	CallR(int position, int ar) :
		m_ar(ar),
		m_position(position)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false);  // TODO

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


class MakeValP: public Operation {
private:
	int m_val;		// int only for now

public:
	MakeValP(int val) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false); // TODO
	}
};


class MakeValR: public Operation {
private:
	int m_val;		// int only for now

public:
	MakeValR(int val) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false); // TODO
	}
};


class Fetch: public Operation {
private:
	int m_pos{0};

public:
	// For the time being, only control level 0
	Fetch(int pos) :
		m_pos(pos)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false); // TODO
		//interpreter.ret(m_pos);
	}
};

class FetchP: public Operation {
private:
	int m_pos{0};

public:
	// For the time being, only control level 0
	FetchP(int pos) :
		m_pos(pos)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false); // TODO
		//interpreter.ret(m_pos);
	}
};

//////////////////////////////
// Old operations
//////////////////////////////






class GoFalse: public Operation {
private:
	int m_target;
	int m_val2;

public:
	GoFalse(int target, int val2) :
		m_target(target),
		m_val2(val2)
	{
	}

	void execute(Interpreter &interpreter) override {
		assert(false);  // TODO

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



#endif // OPERATIONS_H
