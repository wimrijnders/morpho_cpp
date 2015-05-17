#ifndef ARHANDLER_H
#define ARHANDLER_H
#include "ActivationRecord.h"

class ARHandler {
private:
	ActivationRecord *m_activation_record{nullptr};

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

protected:
  static void swap(ARHandler &first, ARHandler &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    assert(first.m_activation_record == nullptr);

    // by swapping the members of two classes,
    // the two classes are effectively swapped
    swap(first.m_activation_record, second.m_activation_record);
  }

  void init() {
    assert(m_activation_record == nullptr);
    m_activation_record = new ActivationRecord();
  }

	/**
	 * @return true if can run next activation record,
	 *         false if no more activation to run.
	 */
	bool return_function(int &pc) {
		if (m_activation_record == nullptr) {
			return false;
		}

		auto *tmp = m_activation_record;

		pc = tmp->return_address();

		m_activation_record = tmp->control_link();
		delete tmp;

		// Perhaps TODO: set new instruction array.
		// Perhaps there is a single large array, so that setting is not necessary.
		return true;
	}


	void clear() {
		m_activation_record = nullptr;
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


	/**
	 * @brief Init activation record for a regular call
	 */
	void call_init(int index, int pc) {
		// NOTE: program counter not set here!!!!!
		ActivationRecord	*ar = init_ar(index);

		ar->activate(pc + 1, m_activation_record);

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
	void become(int index, int &pc) {
		// NOTE: program counter not set here!!!!!
		ActivationRecord	*ar = init_ar(index);

		// Set return variables to those of previous ar
		ar->activate(
			m_activation_record->return_address(),
			m_activation_record->control_link()
		);
		// Remove previous ar
		pop_ar(0, pc);

		m_activation_record = ar;
	}


	void pop_ar(int level, int &pc) {
		assert(level == 0);		// Deeper levels not handled yet

		ActivationRecord *ar = control_link(level);
		assert(ar == m_activation_record);

		ActivationRecord *tmp;
		m_activation_record->deactivate(pc, tmp);
		delete m_activation_record;
		m_activation_record = tmp;

		// Substract 1 from program counter because it gets incremented in the loop
		pc--;
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


	bool ar_empty() const {
		return (m_activation_record == nullptr);
	}

public:
	ARHandler() = default;

	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	ARHandler(const ARHandler &rhs) {
		// Only allow this if not assigned yet.
		assert(m_activation_record == nullptr);

		m_activation_record = rhs.m_activation_record;
	}

	~ARHandler() {
		assert(m_activation_record == nullptr);	 // Would really prefer for this to be the case
		delete m_activation_record;
	}

};

#endif // ARHANDLER_H
