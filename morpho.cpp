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



#include "Interpreter.h"


class ActivationRecord;

class StoreArgVal: public Instruction {
private:
	int m_val{0};
	int m_pos2{0};

public:
	StoreArgVal(int ar, int pos, int val) :
		Instruction(ar, pos),
		m_val(val)
	{}


	void execute(Interpreter &interpreter) override {
		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, new IntObject(m_val));
	}
};

class StoreArgVar: public Instruction {
private:
	int m_lev2{0};
	int m_pos2{0};

public:
	StoreArgVar(int ar, int pos, int lev2, int pos2) :
		Instruction(ar, pos),
		m_lev2(lev2),
		m_pos2(pos2)
	{}


	/**
	 * Assume level 0 if not specified
	 */
	StoreArgVar(int ar, int pos, int pos2) :
		StoreArgVar(ar, pos, 0, pos2) 
	{}

	void execute(Interpreter &interpreter) override {
		ActivationRecord *from_ar = interpreter.control_link(m_lev2);
		AnyObject *from_var = from_ar->get(m_pos2);
		assert(from_var != nullptr);

		ActivationRecord *ar = interpreter.set_ar(m_ar);
		ar->set(m_pos, from_var->clone());
	}
};

class Return: public Instruction {
private:
	int m_lev{0};

public:
	// For the time being, only control level 0
	Return() :
		Instruction(0,0),
		m_lev(0)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.pop_ar(m_lev);
	}
};

using lib_func = void (Interpreter &);

class Call: public Instruction {
	lib_func *m_func{nullptr};

public:
	Call(lib_func &func, int ar) : 
		Instruction(ar, 0),
		m_func(func)
	{
	}

	void execute(Interpreter &interpreter) override {
		interpreter.push_ar(m_ar);

		m_func(interpreter);
	}
};

/**
 * Library definitions.
 *
 * These are called directly, not bothering with the morpho assembly code syntax here
 */

// Int's only for now.
void smaller_equal_two(Interpreter &interpreter) {
	AnyObject *var1 = interpreter.cur_ar()->get(0);
	assert(var1 != nullptr);
	IntObject *int1 = dynamic_cast<IntObject *>(var1);

	AnyObject *var2 = interpreter.cur_ar()->get(1);
	assert(var2 != nullptr);
	IntObject *int2 = dynamic_cast<IntObject *>(var2);

	// return value in accumulator
	interpreter.set_acc(new BoolObject(int1->val() <= int2->val()));

	// Return from function
	Return ret;
	ret.execute(interpreter);
}


/**
 * Example program from chapter Example in second doc.
 *
 * Doing the assembly code only; we won't even bother with the actual language.
 *
 * The intention here is to implement the least possible functionality to make it
 * work, so that the VM can be tested.
 * Filling it in as we go along.
 */
Instruction *data[] = {
	new StoreArgVar(-1, 0, 0),
	new StoreArgVal(-1, 1, 2),
	new Call(smaller_equal_two, -1),		// Cheating here; not intending to
																			// implement lib functions in morpho assembly
	nullptr
};

InstructionArray example(data);


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
