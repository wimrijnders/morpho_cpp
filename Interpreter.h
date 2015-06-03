#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <sstream>
#include <memory>
#include <exception>
#include <string>
#include "Continuation.h"
#include "Closure.h"
#include "common/Operation.h"


class Error : public std::exception {
private:
   std::string s;

public:
   Error() {}
   Error(std::string ss) : s(ss) {}
   ~Error() throw () {}
   void msg(std::string ss) { s = ss; }

   const char* what() const throw() { return s.c_str(); }
};



class Interpreter : public Continuation {

private:

	// NOTE: the accumulator is the first argument (index 0) when making a call.
	//       Acc value is pushed as last on the variables stack when nargs >= 1.
	std::unique_ptr<AnyObject> m_accumulator;

	bool m_dead{true};


  void swap(Interpreter &first, Interpreter &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap; 

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_accumulator, second.m_accumulator); 
    Continuation::swap(first, second);
    swap(first.m_dead, second.m_dead);
  }


public:
	Interpreter(OperationArray *code) {
		assert(code != nullptr);

		m_code = code;
		m_pc = 0;
		m_dead = false;
	}


	~Interpreter() {
		// Note that program pointed to (m_code) is not deleted.
		// At time of writing, this is handled extern.
	}


	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	Interpreter(const Interpreter &rhs) {
		// Only allow this if not assigned yet.
		assert(m_accumulator.get() == nullptr);
		assert(m_code == nullptr);

		// assignment to acc intentionally skipped here
		m_code = rhs.m_code;
		m_pc   = rhs.m_pc;
		m_dead = rhs.m_dead;
	}

	Interpreter(Interpreter &&rhs) /* : Interpreter() */ {
		Interpreter::swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
		Interpreter::swap(*this, rhs);
    return *this;
	}

	void clear() {
		m_code = nullptr;
	}

	AnyObject *get_acc() {
		return m_accumulator.get();
	}

	void set_acc(AnyObject *obj) {
		m_accumulator.reset(obj);
	}



 /**
  * @brief Perform absolute jump in current operations array
  *
  * NOTE: All jumps are relative in morpho2, so we should conform in due time.
  *       However, absolute jumps are more convenient
  */
	void jump_absolute(int target) {
		int new_pc = target;

		// Substract 1 from program counter because it gets incremented in the loop
		new_pc--;

		assert(new_pc >= 0 && (unsigned) new_pc < m_code->size());

		m_pc = new_pc;
	}

	void call(int offset, int nenv, int narg)	{
		// Built-ins should be called directly, not through this method

		push_return_continuation(narg);
		jump_relative(offset);

		fixStackForCall(m_accumulator.get(), nenv,narg);
	}

	/**
	 * @brief As call(), but doesn't push a continuation.
	 *
	 * Instead, it uses the current m_ret to return.
	 */
	void become(int offset, int nenv, int narg) {
		jump_relative(offset);

		fixStackForCall(m_accumulator.get(), nenv,narg);
	}

	void doFetch(int pos) {
		m_accumulator.reset(fetch(pos));
	}


  /**
   * @brief Push value of accumulator on the stack
   */
  void push() {
    Continuation::push(m_accumulator.get());
  }


	/////////////////////////////////////////////
	// Low level support of common operations
	/////////////////////////////////////////////

	/**
	 * @brief Return from current call.
	 *
	 * Can't use 'return' as name because reserved keyword.
	 *
	 * This relies on the proper initialization of the 'current'
	 * activation record; i.e. return address and return activation record
	 * are set in current.
	 *
	 * If that's the case, only the return activation record needs to be set
	 * to current.
	 */
	void ret(int level = 0) {
		assert(level == 0); // Only top-level for now

		m_dead = !call_ret();
	}

	bool is_dead() {
		return m_dead;
	}


	/**
	 * @brief Main loop for this interpreter
	 */
	void loop() {
		assert(m_code != nullptr);
		

		while(true) {
			(*m_code)[m_pc]->execute(*this);
			m_pc++;


			// If we reached end of code, stop
			if ((*m_code)[m_pc] == nullptr) {
				break;
			}

			// Top-level return.
			if (is_dead()) {
				break;
			}
		}
	}


	AnyObject *get_arg(int narg) {
		AnyObject *tmp = nullptr;

		if(narg == 0) {
			tmp = m_accumulator.get();
		} else {
				tmp = stack().get_arg(narg);
		}

		return tmp;
	}


	void callClosure(int narg);
};

#endif // INTERPRETER_H
