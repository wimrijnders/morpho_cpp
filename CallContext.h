#ifndef CALLCONTEXT_H
#define CALLCONTEXT_H

class Interpreter;

using lib_func = void (Interpreter &);

class CallContext {
protected:
	int m_nargs{-1};
	int m_nenv{-1};
	lib_func *m_func{nullptr};
	int m_offset{0};


public:
	/**
	 * @brief Init with library function
	 */
	CallContext(lib_func &func, int nargs = 0, int nenv = 0) :
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
	CallContext(int offset, int nargs = 0, int nenv = 0) :
		m_nargs(nargs),
		m_nenv(nenv),
		m_offset(offset)
	{}

	int narg() { return m_nargs; }

	bool call_builtin(Interpreter &interpreter);
	void call(Interpreter &interpreter);
	void become(Interpreter &interpreter);
};


#endif // CALLCONTEXT_H
