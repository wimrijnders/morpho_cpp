#ifndef FIBERSTATE_H
#define FIBERSTATE_H
#include "Continuation.h"


class FiberState : public Continuation {
private:
	// NOTE: the accumulator is the first argument (index 0) when making a call.
	//       Acc value is pushed as last on the variables stack when nargs >= 1.
	ObjectRef m_accumulator;

protected:

  static void swap(FiberState &first, FiberState &second) /* nothrow */ {
    using std::swap;

    swap(first.m_accumulator, second.m_accumulator);
    Continuation::swap(first, second);
  }

public:

	ObjectRef &get_acc() {
		return m_accumulator;
	}

	void set_acc(AnyObject *obj) {
		if (obj == nullptr) {
			m_accumulator.reset();
		} else {
			// Copy of the object for the pedantic case
			// that source (operation) disappears while the object
			// is still used.
			m_accumulator.reset(obj->clone());

		}

	}

	void set_acc(ObjectRef &obj) {
		m_accumulator = obj;
	}

	void call(int offset, int nenv, int narg)	{
		// Built-ins should be called directly, not through this method

		push_return_continuation(narg);
		jump_relative(offset);

		fixStackForCall(m_accumulator, nenv,narg);
	}


	/**
	 * @brief As call(), but doesn't push a continuation.
	 *
	 * Instead, it uses the current m_ret to return.
	 */
	void become(int offset, int nenv, int narg) {
		jump_relative(offset);

		fixStackForCall(m_accumulator, nenv,narg);
	}


	void doFetch(int pos) {
		m_accumulator = fetch(pos);
	}


  /**
   * @brief Push value of accumulator on the stack
   */
  void push() {
    Continuation::push(m_accumulator /*.get() */);
  }

	ObjectRef get_arg(int narg) {
		ObjectRef tmp;

		if(narg == 0) {
			tmp = m_accumulator;
		} else {
				tmp = stack().get_arg(narg);
		}

		return tmp;
	}

	void callClosure(int narg);
};

#endif // FIBERSTATE_H
