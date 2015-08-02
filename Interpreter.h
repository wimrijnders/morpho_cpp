#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <sstream>
#include <memory>
//#include <string>
#include "FiberState.h"
#include "Closure.h"
#include "common/Operation.h"



class Interpreter : public FiberState {

private:


	bool m_dead{true};


  void swap(Interpreter &first, Interpreter &second) /* nothrow */ {
    using std::swap; 

    FiberState::swap(first, second);
    swap(first.m_dead, second.m_dead);
  }


public:
	Interpreter(OperationArray *code) {
		assert(code != nullptr);

		m_code = code;
		m_pc = 0;
		m_dead = false;
	}


	Interpreter(const FiberState &fs) :
	 FiberState(fs),
		m_dead(false)
	{
		assert(m_code != nullptr);
	}


	~Interpreter() {
		// Note that program pointed to (m_code) is not deleted.
		// At time of writing, this is handled extern.
	}


	// Needed for correct compilation.
	Interpreter(const Interpreter &rhs) {
		// Only allow this if not assigned yet.
		assert(get_acc().get() == nullptr);
		assert(m_code == nullptr);

		// assignment to acc intentionally skipped here
		m_code = rhs.m_code;
		m_pc   = rhs.m_pc;
		m_dead = rhs.m_dead;
	}


	Interpreter(Interpreter &&rhs){
		Interpreter::swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
		Interpreter::swap(*this, rhs);
    return *this;
	}


	void clear() {
		m_code = nullptr;
	}



#if 0
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
#endif



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


	void loop();
	void callClosure(int narg);
};

#endif // INTERPRETER_H
