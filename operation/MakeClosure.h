#ifndef MAKECLOSURE_H
#define MAKECLOSURE_H
#include <cassert>
#include "../StackLink.h"
#include "../Closure.h"
#include "../common/Operation.h"

namespace operation {

/**
 * @brief Perform a call to either a built-in function or
 *        a position in the current operations array.
 *
 */
class MakeClosure: public Operation {
private:
	int m_narg{-1};
	int m_nenv{-1};
	lib_func *m_func{nullptr};
	int	m_fun_offset{-1};
	int m_go_offset{-1};

public:
	/**
	 * @brief Init with library function
	 */
	MakeClosure(lib_func &func, int narg, int nenv, int go_offset) :
		m_narg(narg),
		m_nenv(nenv),
		m_func(func),
		m_go_offset(go_offset)
	{}


	/**
	 * @brief Init with index into the given instruction array
	 *
	 * The position is passed as a relative jump from the current
	 * program counter.
	 */
	MakeClosure(int fun_offset, int narg, int nenv, int go_offset) :
		m_narg(narg),
		m_nenv(nenv),
		m_func(nullptr),
		m_fun_offset(fun_offset),
		m_go_offset(go_offset)
	{}


	void execute(Interpreter &interpreter)	{
		StackLink env = interpreter.getEnvironment(m_nenv);

		if (m_func != nullptr) {
			assert(m_fun_offset == 0);
			interpreter.set_acc(new Closure(m_fun_offset,env,m_nenv,m_narg,interpreter.code()));
		} else {
			assert(m_fun_offset != 0);
			interpreter.set_acc(new Closure(interpreter.getPC()+m_fun_offset,env,m_nenv,m_narg,interpreter.code()));
		}

		interpreter.jump_relative(m_go_offset);
	}

};

using MakeClosureP = OperationP<MakeClosure>;
//using MakeClosureR = OperationR<MakeClosure>;

#if 0
class MakeClosureP: public MakeClosure {
public:
	using MakeClosure::MakeClosure;

	void execute(Interpreter &interpreter) override {
		interpreter.push();
		MakeClosure::execute(interpreter);
	}
};
#endif

#endif // MAKECLOSURE_H
