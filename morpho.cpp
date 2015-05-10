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
#include "Runnable.h"
#include "Operations.h"
#include "library_functions.h"

using namespace operation;
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
  /**
   * NOTE: for all jumps and calls, the value to jump to is 'one before',
   *       because program counter is always incremented.
   *       For libraries, the program counter is not used, so then this offset doesn't occur.
   */

// Entry point: fib(3)
	new StoreArgVal(-1, 0, 3),
	new Call(2, -1),										// Jump only within current instruction array
	new Return(),

// Start of function fib(n)
	new StoreArgVar(-1, 0, 0),
	new StoreArgVal(-1, 1, 2),
	new Call(smaller_equal_two, -1),		// Cheating here; not intending to
																			// implement lib functions in morpho assembly
	new GoFalse(2),	                    // Jump three(!) instructions forward.
	new MakeVal(1),
	new Return(),

	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 1),
	new Call(subtract, -3),							// Cheating again
	new StoreArgAcc(-2, 0),
	new Call(2, -2),										// Jump only within current instruction array
	new StoreArgAcc(-1, 0),
	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 2),
	new Call(subtract, -3),							// Cheating again
	new StoreArgAcc(-2, 0),
	new Call(2, -2),										// Jump only within current instruction array
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

		loop();

		// On return function:
		if (!return_function()) {
			set_done();
		}
	}

public:
	Fiber(InstructionArray *program) :
		Interpreter(program)
	{
		assert(program != nullptr);
		assert(program->size() > 0);

		m_state = READY;
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
public:
	void add(const Fiber &item) {
		RunnableList<Fiber>::add(item);

		m_state = READY;
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
	ThreadPool() : std::vector<Thread>(NUM_WORKER_THREADS)  {
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
	void add_code(InstructionArray *code) {
		Fiber fiber(code);
		Process process;
		process.add(fiber);
		add(process);

		// Remove ptr's after copy ctor call in process
		process.clear();
		fiber.clear();
	}

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
	vm.add_code(&example);

	vm.start();

	return 0;
}
