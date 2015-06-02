#ifndef CALL_H
#define CALL_H

namespace operation {


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

}  // namespace operation

#endif // CALL_H
