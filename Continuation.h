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

public:
  StackLink &stack() { return m_stack; }
};


#endif // CONTINUATION_H
