#ifndef RUNCONTEXT_H
#define RUNCONTEXT_H
#include <cassert>
#include <memory>  // swap()
#include "Stack.h"
#include "common/Operation.h"

class RunContext {
private:

	Stack m_stack;

protected:
	OperationArray *m_code{nullptr};
	int m_pc{-1};

  static void swap(RunContext &first, RunContext &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    assert(first.m_code == nullptr);

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_code, second.m_code);
    swap(first.m_pc, second.m_pc);
  }


  void set_stack(const Stack &stack) {
    m_stack = stack;
  }


  void fixStackForCall(ObjectRef &acc, int nenv, int narg) {
    m_stack.fixStackForCall(acc, nenv, narg);
  }


	void fixStackForClosureCall(ObjectRef &acc, Stack &env, int narg ) {

		m_stack.fixStackForClosureCall(acc, env, narg);
	}

	ObjectRef &fetch(int pos) {
		return m_stack.fetch(pos);
	}


  void push(ObjectRef &obj) {
    m_stack.push(obj);
  }

public:

	void store(ObjectRef &obj, int pos) {
		m_stack.store(obj, pos);
	}


  void drop(int nargs) {
    if (nargs > 0) {
      m_stack.pop(nargs);
    }
  }


  ObjectRef pop() {
    return m_stack.pop();
  }


	Stack &stack() { return m_stack; }


	Stack getEnvironment(int nenv) const {
		return m_stack.get_env(nenv);
	}


	OperationArray *code() { return m_code; }


	void jump_relative(int target) {
		int new_pc = m_pc + target;

		// Substract 1 from program counter because it gets incremented in the loop
		new_pc--;

		assert(new_pc >= 0 && (unsigned) new_pc < m_code->size());

		m_pc = new_pc;
	}

	int getPC() { return m_pc; }
};

#endif // RUNCONTEXT_H
