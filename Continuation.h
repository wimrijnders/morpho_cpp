#ifndef CONTINUATION_H
#define CONTINUATION_H
#include "RunContext.h"

class Continuation : public RunContext {
private:

	// Used for debugging and/or interactive environment StackLink[] m_display;
	Continuation *m_ret{nullptr};
	Continuation *m_ex{nullptr};


protected:

  static void swap(Continuation &first, Continuation &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    RunContext::swap(first, second);

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_ret, second.m_ret);
    swap(first.m_ex, second.m_ex);
  }

	void disconnect() {
		// Pre: continuation copied beforehand to current state
		m_ret = nullptr;
		m_ex = nullptr;

		//RunContext::disconnect();
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


	/**
	 * @param narg The number of parameters to skip in the saved continuation.
	 */
	Continuation *clone(int narg = 0) {
		Continuation *tmp = new Continuation(*this);

    // Note that one less than narg is stored.
    // This is analogous to java code; apparently, the acc
    // is regarded as the first parameter.
    tmp->drop(narg - 1);

    return tmp;
  }


public:

	/**
	 * @param narg The number of parameters to skip in the saved continuation.
	 */
	void push_return_continuation(int narg = 0) {
		m_ret = clone(narg);
	}
};


#endif // CONTINUATION_H
