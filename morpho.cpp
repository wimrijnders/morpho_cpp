/**
 * Elaboration in code of the text about Morpho VM
 *
 * Intended to understand the text, but who knows what might come from it....
 *
 * Compiles with:
 *
 *  g++ -std=c++11 morpho.cpp -o morpho
 */
#include <cassert>
#include <atomic>
#include <string>
#include <thread>
#include <vector>

using std::vector;
using std::string;

#define USE_THREADS

//////////////////////////
// Support methods
//////////////////////////

/**
 * @brief Remove first item in list without returning its value
 *
 * Following is deemed inefficient by stackoverflow
 * Apparently, deque is better for popping, or use pop_back() instead.
 *
 * TODO: Find more efficient method later on.
 */
template <typename V>
void pop(std::vector<V> & v) {
	assert(!v.empty());

	v.erase(v.begin());
}

//////////////////////////
// Morpho transcription 
//////////////////////////

class Runnable {
public:
	enum /* class */ State {
		INITIALIZING,
		READY,
		RUNNING,
		YIELDED,
		PAUSED,			// Yield after executing a fair number of instructions
		DONE
	};

private:

	virtual void run_intern() = 0;

protected:
	State m_state{INITIALIZING};

	void set_done() { m_state = DONE; }

public:
	State state() { return m_state; }
	bool can_run() { return m_state == READY || m_state == PAUSED; }
	bool done() { return m_state == DONE; }

	void run() {
		assert(m_state == READY || m_state == PAUSED);

		m_state = RUNNING;

		run_intern();

		// Post: set state to either YIELDED, PAUSED or DONE
		m_state = DONE; // YIELDED, PAUSED
	}
};


template<typename RunnableItem>
class RunnableList {
private:
	std::vector<RunnableItem> m_ready;
	std::vector<RunnableItem> m_running;

	void remove_done() {
		// Clean up
		auto i = std::begin(m_running);

		while (i != std::end(m_running)) {
	    // do some stuff
	    if (i->done())
	        i = m_running.erase(i);
	    else
	        ++i;
		}
	}


	virtual bool execute_intern(RunnableItem &runnable) = 0;

protected:

	void execute() {
		for(auto &runnable: m_running) {
			if (!runnable.can_run()) continue;

			if(!execute_intern(runnable)) break;
		}

		remove_done();
	}


	void loop() {
		// Initialize worker threads here

		while(true) {
			while (!m_ready.empty()) {
				m_running.push_back(m_ready[0]);
				pop(m_ready);
			}

			execute();

			if (m_ready.empty() && m_running.empty()) {
				break;
			}
		}
	}
};




class Interpreter;

class Instruction {
public:
	void execute(Interpreter &interpreter) {
	}
};


class InstructionArray : public std::vector<Instruction> {
};


/**
 * Proxy for later handling.
 */
class AnyObject {
};

class Environment {
public:
	// Convention in VM: temp have negative indexes, local >= 0
	vector<AnyObject> m_temp_variables;
	vector<AnyObject> m_local_variables;

	// Environment are chained, from direct enclosing to top-level global.
	Environment &m_enclosing_environment;
};


/**
 * Currently, instances form linked lists.
 */
class ActivationRecord {
private:
	int m_return_address;

	// activation record to use when returning from the current function
	ActivationRecord *m_control_link{nullptr};

	Environment m_environment;

public:
	~ActivationRecord() {
		delete m_control_link;
	}

	int return_address() { return m_return_address; }
	ActivationRecord *control_link() { return m_control_link; }
};


class Interpreter {
private:
	//registers
	AnyObject m_accumulator;
	ActivationRecord *m_activation_record{nullptr};
	InstructionArray *m_program{nullptr};	// Many arrays present?
	int m_program_counter;

  friend void swap(Interpreter &first, Interpreter &second) /* nothrow */ {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap; 

		assert(first.m_activation_record == nullptr);
		assert(first.m_program== nullptr);

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
		delete m_activation_record;
	}


	// Not sure about copying the m_activation_record here.
	// But this is needed for correct compilation
	Interpreter(const Interpreter &rhs) = default;

	Interpreter(Interpreter &&rhs) /* : Interpreter() */ {
  	swap(*this, rhs);
	}

	Interpreter &operator=(Interpreter &&rhs) {
  	swap(*this, rhs);
	}


	void loop() {
		assert(m_program != nullptr);
		

		while(true) {
			(*m_program)[m_program_counter++].execute(*this);
		}
	}
};


class Fiber: public Runnable, public Interpreter {
private:

	void suspend() {
		assert(m_state == RUNNING);	// Might be too strict; eg. already suspended?

		m_state = PAUSED;
	}

	void resume() {
		assert(m_state == PAUSED || m_state == YIELDED);	// Might be too strict

		m_state = RUNNING;
	}

	void run_intern() override {
		// TODO!

		// On return function:
		if (!return_function()) {
			set_done();
		}
	}
};


class Process: public Runnable, public RunnableList<Fiber> {
private:

	// registers

	virtual bool execute_intern(Fiber &fiber) override {
		fiber.run();
	}

	void run_intern() override {
		loop();
		set_done();
	}
};


class Thread {
private:
	std::thread m_thread;

	std::atomic<bool> m_running{false};

public:
	bool available() { return m_running == false; }

	void execute(Process &process) {
		assert(!m_running);

		// TODO: determine if there's a race condition, even with use of atomic.
		m_running = true;

		m_thread = std::thread(&Process::run, &process);

		// TODO: If yielded or paused, reschedule

		m_running = false;
	}

	void join() { m_thread.join(); }
};


class ThreadPool: public std::vector<Thread> {
private:

public:

	/**
	 * @brief Return an available thread, if present
	 *
	 * @return pointer to Thread instance if one available, null otherwise.
	 */
	Thread *get_available() {
		for (auto &thr: *this) {
			if (thr.available()) {
				return &thr;
			}
		}

		return nullptr;
	}


	void join() {
		for (auto &thr: *this) {
			thr.join();
		}
	}
};


/**
 * VM can be regarded as a thread itself
 *
 * In the text, it is run by the 'main' thread, and can execute
 * processes if multi-processing not required.
 */
class VM: public RunnableList<Process> {
private:
	ThreadPool           m_thread_pool;

	bool execute_intern(Process &process) override { 
#ifdef USE_THREADS
		Thread *thr = m_thread_pool.get_available();
		if (thr == nullptr) return false;;

		// Run Processin context of thread
		thr->execute(process);
#else
		process.run();
#endif
		return true;
	}

public:
	void start() {
		// Initialize worker threads here

		loop();

		m_thread_pool.join();
	}
};


//////////////////////////
// Entry point
//////////////////////////

int main(int argc, char *argv[]) {
	VM vm;

	// load program here

	vm.start();
}
