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

const int NUM_WORKER_THREADS = 0;

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


#include "Operations.h"
#include "library_functions.h"


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
// Entry point: fib(3)
	new StoreArgVal(-1, 0, 3),
	new Call(2, -2),										// Jump only within current instruction array
	new Return(),

// Start of function fib(n)
	new StoreArgVar(-1, 0, 0),
	new StoreArgVal(-1, 1, 2),
	new Call(smaller_equal_two, -1),		// Cheating here; not intending to
																			// implement lib functions in morpho assembly
	new GoFalse(3),	// Jump three instructions forward.
	new MakeVal(1),
	new Return(),

	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 1),
	new Call(subtract, -3),							// Cheating again
	new StoreArgAcc(-2, 0),
	new Call(3, -2),										// Jump only within current instruction array
	new StoreArgAcc(-1, 0),
	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 2),
	new Call(subtract, -3),							// Cheating again
	new StoreArgAcc(-2, 0),
	new Call(3, -2),										// Jump only within current instruction array
	new StoreArgAcc(-1, 1),
	new Become(add, -1),							  // Cheating again
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
		return true;
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
/*
	// grumbl deleted functions
	Thread() = default;
	Thread(const Thread &thr) {
		m_thread = thr.m_thread;
		m_running = thr.m_running;
	}
	Thread operator=(const Thread &thr) {
		m_thread = thr.m_thread;
		m_running = thr.m_running;

		return *this;
	}
*/
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
	ThreadPool() : std::vector<Thread>(NUM_WORKER_THREADS)  {
//		for (int i = 0; i < NUM_WORKER_THREADS; ++i) {
//			Thread tmp;
//			push_back(tmp);
//		}
	}

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
		if (NUM_WORKER_THREADS > 0) {
			Thread *thr = m_thread_pool.get_available();
			if (thr == nullptr) return false;;

			// Run Processin context of thread
			thr->execute(process);
		} else {
			process.run();
		}

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

	return 0;
}
