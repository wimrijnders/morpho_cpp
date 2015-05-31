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
	int m_nenv{0};
	lib_func *m_func{nullptr};
	int m_offset{0};

public:
	/**
	 * @brief Init with library function
	 */
	Call(lib_func &func, int nargs = 0, int nenv = 0) :
		m_nargs(nargs),
		m_nenv(nenv),
		m_func(func)
	{}


	/**
	 * @brief Init with index into the current instruction array
	 *
	 * The position is passed as a relative jump from the current
	 * program counter.
	 */
	Call(int offset, int nargs = 0, int nenv = 0) :
		m_nargs(nargs),
		m_nenv(nenv),
		m_offset(offset)
	{}


	void execute(Interpreter &interpreter) override {
		if (m_func != nullptr) {
			assert(m_offset == 0);
			m_func(interpreter);
		} else {
			assert(m_offset != 0);
			interpreter.call(m_offset, m_nenv, m_nargs);
		}
	}
};

class CallR: public Operation {
private:
	int m_nargs{0};
	int m_nenv{0};
	lib_func *m_func{nullptr};
	int m_offset{0};

public:
	// Init with library function
	CallR(lib_func &func, int nargs = 0, int nenv = 0) :
		m_nargs(nargs),
		m_nenv(nenv),
		m_func(func)
	{}

	// Init with index into the current instruction array
	CallR(int offset, int nargs = 0, int nenv = 0) :
		m_nargs(nargs),
		m_nenv(nenv),
		m_offset(offset)
	{}

	void execute(Interpreter &interpreter) override {
		if (m_func != nullptr) {
			// TODO: nargs, nenv neglected for builtin calls!

			assert(m_offset == 0);
			m_func(interpreter);

			// NOTE: in the java code, the acc it temporarily stored
			//       before executing the return. At this time, this
			//       does not appear to be necessary.
			interpreter.ret();
		} else {
			assert(m_offset != 0);
			interpreter.become(m_offset, m_nenv, m_nargs);
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
		// Following is the only difference with MakeVal
		interpreter.push();
		interpreter.set_acc(new IntObject(m_val));
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
		interpreter.set_acc(new IntObject(m_val));
		// Following is the only difference with MakeVal
		interpreter.ret();
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
		interpreter.doFetch(m_pos);
	}
};

class FetchP: public Operation {
private:
	int m_pos{0};

public:
	FetchP(int pos) :
		m_pos(pos)
	{}

	void execute(Interpreter &interpreter) override {
		// NOTE: Following is different from java version!
		//       Perhaps a bug?
		interpreter.push();
		interpreter.doFetch(m_pos);
	}
};


class GoFalse: public Operation {
private:
	int m_target;
	int m_val2;

public:
	GoFalse(int target, int val2) :
		m_target(target),
		m_val2(val2)
	{}

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



#endif // OPERATIONS_H
