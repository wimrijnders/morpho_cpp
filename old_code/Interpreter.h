#ifndef OLD_INTERPRETER_H
#define OLD_INTERPRETER_H
#include <memory>
#include "ARHandler.h"
#include "../common/Operation.h"


class Interpreter : private ARHandler {
public:
	using ARHandler::set_ar;
	using ARHandler::cur_ar;
	using ARHandler::control_link;

	OperationArray *m_code{nullptr};
	int m_pc{-1};

private:
	std::unique_ptr<AnyObject> m_accumulator;

  void swap(Interpreter &first, Interpreter &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap; 

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_accumulator, second.m_accumulator); 
    ARHandler::swap(first, second);
  }


protected:

	/**
	 * @return true if can run next activation record,
   *         false if no more activation to run.
	 */
	bool return_function() {
		return ARHandler::return_function(m_pc);
	}

public:
	Interpreter(OperationArray *code) {
		m_code = code;
		m_pc = 0;
		ARHandler::init();
	}

	~Interpreter() {

		// Note that m_program is not deleted. At time of writing, this is handled extern.
	}


	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	Interpreter(const Interpreter &rhs) :
		ARHandler(rhs)
	{
		// Only allow this if not assigned yet.
		assert(m_accumulator.get() == nullptr);
		assert(m_code == nullptr);

		// assignment to acc intentionally skipped here
		m_code = rhs.m_code;
		m_pc = rhs.m_pc;
	}

	Interpreter(Interpreter &&rhs) /* : Interpreter() */ {
		Interpreter::swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
		Interpreter::swap(*this, rhs);
    return *this;
	}

	void clear() {
		ARHandler::clear();
		m_code = nullptr;
	}

	AnyObject *get_acc() {
		return m_accumulator.get();
	}

	void set_acc(AnyObject *obj) {
		m_accumulator.reset(obj);
	}



public:

	//void pop_ar(int level) {
	//	ARHandler::pop_ar(level, m_pc);
	//}


	void become(int index) {
		ARHandler::become(index, m_pc);
	}


	void call_init(int index) {
		ARHandler::call_init(index, m_pc);
	}

	void jump_relative(int target) {
		int new_pc = m_pc + target;

		// Substract 1 from program counter because it gets incremented in the loop
		new_pc--;

		assert(new_pc >= 0 && (unsigned) new_pc < m_code->size());

		m_pc = new_pc;
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

		pop_ar(level, m_pc);
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
			// If there are no more activation records, we reached the end
			if (ar_empty()) {
				break;
			}
		}
	}
};

#endif // OLD_INTERPRETER_H
