#ifndef INTERPRETER_H
#define INTERPRETER_H
#include <memory>
#include "ActivationRecord.h"
#include "Instruction.h"

class Interpreter {
private:
	//registers
	std::unique_ptr<AnyObject> m_accumulator;

	ActivationRecord *m_activation_record;
	InstructionArray *m_program{nullptr};	// Many arrays present?
	int m_program_counter;

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
	}

public:
	~Interpreter() {
		assert(m_activation_record == nullptr);	 // Would really prefer for this to be the case
		delete m_activation_record;
	}


	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	Interpreter(const Interpreter &rhs) {
		assert(false); // Really, should never be called
	}

	Interpreter(Interpreter &&rhs) /* : Interpreter() */ {
  	swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
  	swap(*this, rhs);
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

	void push_ar(int index) {
		// NOTE: program counter not set yet!!!!

		AnyObject *obj = m_activation_record->get(index);
		assert(obj != nullptr);

		ActivationRecord	*ar = dynamic_cast<ActivationRecord *>(obj);
		assert(ar != nullptr);
		m_activation_record->set(index, nullptr);
		ar->activate(m_program_counter + 1, m_activation_record);

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

	/**
	 * Main loop for this interpreter
	 */
	void loop() {
		assert(m_program != nullptr);
		

		while(true) {
			(*m_program)[m_program_counter++]->execute(*this);
		}
	}
};

#endif // INTERPRETER_H
