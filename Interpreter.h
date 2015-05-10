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


	//
	// 'Become' is a construct to enable tail recursion.
	// I don't understand fully how it works, but I hope to get there by working with it.
	// 'Become' is enabled by the second parameter.
	//
	void push_ar(int index, bool become = false) {
		// NOTE: program counter not set here!!!!!

		AnyObject *obj = m_activation_record->get(index);
		assert(obj != nullptr);

		ActivationRecord	*ar = dynamic_cast<ActivationRecord *>(obj);
		assert(ar != nullptr);
		m_activation_record->set(index, nullptr);

		if (become) {
			ar->activate(m_activation_record->return_address(), m_activation_record);
		} else {
			ar->activate(m_program_counter + 1, m_activation_record);
		}

		m_activation_record = ar;
	}


	void pop_ar(int level) {
		assert(level == 0);		// Deeper levels not handled yet

		ActivationRecord *ar = control_link(level);
		assert(ar == m_activation_record);

		ActivationRecord *tmp;
		m_activation_record->deactivate(m_program_counter, tmp);
		delete m_activation_record;
		m_activation_record = tmp;
	}

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

		assert(new_pc >= 0 && (unsigned) new_pc < m_program->size());

		m_program_counter = new_pc;
	}

	void jump_absolute(int target) {
		int new_pc = target;

		assert(new_pc >= 0 && (unsigned) new_pc < m_program->size());

		m_program_counter = new_pc;
	}

	/**
	 * Main loop for this interpreter
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

			// TODO: The other condition is a top-level return.
		}
	}
};

#endif // INTERPRETER_H
