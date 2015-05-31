#ifndef CONTINUATION_H
#define CONTINUATION_H
#include <memory>  // swap()
#include "StackLink.h"
#include "common/Operation.h"

class Continuation {
private:

	StackLink m_stack;

	// Used for debugging and/or interactive environment StackLink[] m_display;
	Continuation *m_ret{nullptr};
	Continuation *m_ex{nullptr};

	void disconnect() {
		// Pre: continuation copied beforehand to current state
		m_ret = nullptr;
		m_ex = nullptr;
		m_stack.clear();
	}

protected:
	OperationArray *m_code{nullptr};
	int m_pc{-1};

  static void swap(Continuation &first, Continuation &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    assert(first.m_code == nullptr);

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_code, second.m_code);
    swap(first.m_pc, second.m_pc);
  }


  /**
   * @brief set return continuation as current continuation
   * @return false if there was no return continuation, true otherwise.
   */
  bool call_ret() {
    if (m_ret == nullptr) {
      return false;
    } else {
      auto tmp = m_ret;

      *this = *tmp;
      tmp->disconnect();
      delete tmp;
      return true;
    }
  }

  void fixStackForCall(AnyObject *acc, int nenv, int narg) {
    m_stack.fixStackForCall(acc, nenv, narg);
  }

	/**
	 * @param narg The number of parameters to skip in the saved continuation.
	 */
	Continuation *clone(int narg = 0) {
		Continuation *tmp = new Continuation(*this);

    /*
    tmp.m_code  = m_code;  // pointer copy
    tmp.m_pc    = m_pc;
    tmp.m_stack = m_stack;
    tmp.m_ret   = m_ret;   // pointer copy
    tmp.m_ex    = m_ex;    // pointer copy
    */

    // Note that one less than narg is stored.
    // This is analogous to java code; apparently, the acc
    // is regarded as the first parameter.
    if (narg -1 > 0) {
      tmp->m_stack.pop(narg - 1);
    }

    return tmp;
  }

	AnyObject *fetch(int pos) {
		return m_stack.fetch(pos);
	}

  void push(AnyObject *obj) {
    m_stack.push(obj);
  }

public:

  AnyObject *pop() {
    return m_stack.pop();
  }

  StackLink &stack() { return m_stack; }

	/**
	 * @param narg The number of parameters to skip in the saved continuation.
	 */
	void push_return_continuation(int narg = 0) {
		m_ret = clone(narg);
	}
};


#endif // CONTINUATION_H
