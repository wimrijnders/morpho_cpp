#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "../common/Operation.h"
#include "../Interpreter.h"

template<typename Parent>
class OperationP: public Parent {
public:
	using Parent::Parent;

	void execute(Interpreter &interpreter) override {
		interpreter.push();
		Parent::execute(interpreter);
	}
};

template<typename Parent>
class OperationR: public Parent {
public:
	using Parent::Parent;

	void execute(Interpreter &interpreter) override {
		Parent::execute(interpreter);
		interpreter.ret();
	}
};


//////////////////////////////
// Morpho2 operations
//////////////////////////////

using lib_func = void (Interpreter &);


#include "Call.h"
#include "MakeVal.h"
#include "Fetch.h"
#include "MakeClosure.h"

namespace operation {


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


class Go: public Operation {
private:
	int	m_offset{0};
	int m_drops{0};

public:
	Go(int offset, int drops = 0) :
		m_offset(offset),
		m_drops(drops)
	{}

	void	execute(Interpreter &interpreter) {
		interpreter.drop(m_drops);
		interpreter.jump_relative(m_offset);
	}
};


class CallClosure: public Operation {
private:
	int	m_narg{0};

public:
	CallClosure(int narg) :
		m_narg(narg)
	{}

	void	execute(Interpreter &interpreter) {
		interpreter.callClosure(m_narg);
	}
};


class Store: public Operation {
private:
	int m_pos{0};

public:
	Store(int pos) :
		m_pos(pos)
	{}

	void execute(Interpreter &interpreter) override {
		assert(false); //TODO
	}
};

class GoFalse: public Operation {
private:
	int m_target;
	int m_drops;

public:
	GoFalse(int target, int drops = 0) :
		m_target(target),
		m_drops(drops)
	{}

	void execute(Interpreter &interpreter) override {
		// Test acc for false
		AnyObject *var = interpreter.get_acc();
		assert(var != nullptr);
		BoolObject *ret = dynamic_cast<BoolObject *>(var);
		assert(ret != nullptr);

		if (ret->val() == false) {
			interpreter.drop(m_drops);
			interpreter.jump_relative(m_target);
		}
	}
};

class GoTrue: public Operation {
private:
	int m_target;
	int m_drops;

public:
	GoFalse(int target, int drops = 0) :
		m_target(target),
		m_drops(drops)
	{}

	void execute(Interpreter &interpreter) override {
		// Test acc for true
		AnyObject *var = interpreter.get_acc();
		assert(var != nullptr);
		BoolObject *ret = dynamic_cast<BoolObject *>(var);
		assert(ret != nullptr);

		if (ret->val() == true) {
			interpreter.drop(m_drops);
			interpreter.jump_relative(m_target);
		}
	}
};

} // namespace operation



#endif // OPERATIONS_H
