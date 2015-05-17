#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "ActivationRecord.h"
#include "Instruction.h"

class Interpreter {
private:
	//registers
	std::unique_ptr<AnyObject> m_accumulator;

	ActivationRecord *m_activation_record{nullptr};
	InstructionArray *m_program{nullptr};	// Many arrays present?
	int m_program_counter{-1};

  friend void swap(Interpreter &first, Interpreter &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap; 

		assert(first.m_activation_record == nullptr);
		assert(first.m_program == nullptr);

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_accumulator, second.m_accumulator); 
    swap(first.m_activation_record, second.m_activation_record); 
    swap(first.m_program, second.m_program); 
    swap(first.m_program_counter, second.m_program_counter); 
  }

protected:
	/**
	 * @return true if can run next activation record,
   *         false if no more activation to run.
	 */
	bool return_function() {
		if (m_activation_record == nullptr) {
			return false;
		}

		auto *tmp = m_activation_record;

		m_program_counter = tmp->return_address();

		m_activation_record = tmp->control_link();
		delete tmp;

		// Perhaps TODO: set new instruction array.
		// Perhaps there is a single large array, so that setting is not necessary.
		return true;
	}

public:
	Interpreter(InstructionArray *program) {
		m_program = program;
		m_activation_record = new ActivationRecord();
		m_program_counter = 0;
	}

	~Interpreter() {
		assert(m_activation_record == nullptr);	 // Would really prefer for this to be the case
		delete m_activation_record;

		// Note that m_program is not deleted. At time of writing, this is handled extern.
	}


	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	Interpreter(const Interpreter &rhs) {
		// Only allow this if not assigned yet.
		assert(m_activation_record == nullptr);
		assert(m_accumulator.get() == nullptr);
		assert(m_program == nullptr);
		// assignment to acc intentionally skipped here
		m_activation_record = rhs.m_activation_record;
		m_program = rhs.m_program;
		m_program_counter = rhs.m_program_counter;
	}

	Interpreter(Interpreter &&rhs) /* : Interpreter() */ {
  	swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
  	swap(*this, rhs);
    return *this;
	}

	void clear() {
		m_activation_record = nullptr;
		m_program = nullptr;
	}

	AnyObject *get_acc() {
		return m_accumulator.get();
	}

	void set_acc(AnyObject *obj) {
		m_accumulator.reset(obj);
	}


	ActivationRecord *get_ar(int index) {
		assert(m_activation_record != nullptr);

		AnyObject *obj = m_activation_record->get(index);

		if (obj == nullptr) {
			return nullptr;
		}

		ActivationRecord *ar = dynamic_cast<ActivationRecord *>(obj);
		assert(ar != nullptr);
		return ar;
	}

	ActivationRecord *set_ar(int index) {
		assert(m_activation_record != nullptr);

		ActivationRecord *out_ar = nullptr;

		AnyObject *obj = m_activation_record->get(index);

		if (obj == nullptr) {
			out_ar = new ActivationRecord();
			m_activation_record->set(index, out_ar);
		} else {
			out_ar = dynamic_cast<ActivationRecord *>(obj);
			assert(out_ar != nullptr);
		}

		return out_ar;
	}

	ActivationRecord *cur_ar() {
		assert(m_activation_record != nullptr);
		return m_activation_record;
	}

private:

	/**
	 * @brief Ready activation record in variable for use as current.
	 */
	ActivationRecord	*init_ar(int index) {
		AnyObject *obj = m_activation_record->get(index);
		assert(obj != nullptr);

		ActivationRecord	*ar = dynamic_cast<ActivationRecord *>(obj);
		assert(ar != nullptr);

		// Clear the variable that contained it.
		m_activation_record->set(index, nullptr);

		return ar;
	}

public:

	/**
	 * @brief Init activation record for a regular call
	 */
	void call_init(int index) {
		// NOTE: program counter not set here!!!!!
		ActivationRecord	*ar = init_ar(index);

		ar->activate(m_program_counter + 1, m_activation_record);

		m_activation_record = ar;
	}

	/**
	 * @brief Init activation record for tail recursion.
	 *
	 * 'Become' is a construct to enable tail recursion.
	 * The new activation record takes over the return
	 * parameters from the current activation record.
	 * The new activation record replaced the current one, which is
	 * discarded.
	 *
	 * This eliminates a stack of activation records pointing to the
	 * same return address, which is convenient for deeply recursive
	 * calls.
	 */
	void become(int index) {
		// NOTE: program counter not set here!!!!!
		ActivationRecord	*ar = init_ar(index);

		// Set return variables to those of previous ar
		ar->activate(
			m_activation_record->return_address(),
			m_activation_record->control_link()
		);
		// Remove previous ar
		pop_ar(0);

		m_activation_record = ar;
	}

private:

	void pop_ar(int level) {
		assert(level == 0);		// Deeper levels not handled yet

		ActivationRecord *ar = control_link(level);
		assert(ar == m_activation_record);

		ActivationRecord *tmp;
		m_activation_record->deactivate(m_program_counter, tmp);
		delete m_activation_record;
		m_activation_record = tmp;

		// Substract 1 from program counter because it gets incremented in the loop
		m_program_counter--;
	}

public:

	ActivationRecord *control_link(int level) {
		assert(m_activation_record != nullptr);
		assert(level >=0);

		ActivationRecord *ptr = m_activation_record;

		while (level > 0) {
			assert(ptr != nullptr);
			ptr = ptr->control_link();
			level--;	
		}

		return ptr;
	}


	void jump_relative(int target) {
		int new_pc = m_program_counter + target;

		// Substract 1 from program counter because it gets incremented in the loop
		new_pc--;

		assert(new_pc >= 0 && (unsigned) new_pc < m_program->size());

		m_program_counter = new_pc;
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

		assert(new_pc >= 0 && (unsigned) new_pc < m_program->size());

		m_program_counter = new_pc;
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

		pop_ar(level);
	}

	/**
	 * @brief Main loop for this interpreter
	 */
	void loop() {
		assert(m_program != nullptr);
		

		while(true) {
			(*m_program)[m_program_counter]->execute(*this);
			m_program_counter++;


			// If we reached end of code, stop
			if ((*m_program)[m_program_counter] == nullptr) {
				break;
			}

			// Top-level return.
			// If there are no more activation records, we reached the end
			if (m_activation_record == nullptr) {
				break;
			}
		}
	}
};

#endif // INTERPRETER_H
