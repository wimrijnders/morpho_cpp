#ifndef CALLCONTEXT_H
#define CALLCONTEXT_H

class Interpreter;
class Task;

using lib_func = void (Interpreter &);
using task_func = void (Task &);

/**
 * @brief Call in-memory or built-in method.
 *
 * This class is becoming increasingly ugly; now with two
 * different types of in-built method pointers.
 *
 * TODO: Find a better implementation.
 */
class CallContext {
protected:
	int m_nargs{-1};
	int m_nenv{-1};
	lib_func  *m_func{nullptr};
	task_func *m_task_func{nullptr};
	int        m_offset{0};

	bool call_task_func(/* Task &task */);

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
