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
#include <iostream>
#include <vector>
#include "Runnable.h"
#include "Operations.h"
#include "library_functions.h"

using namespace operation;
using std::cout;
using std::endl;
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
   * NOTE:
   *
   * - Jumps and calls are only within current instruction array
   * - For built-in fucntions, the program counter is not used
   * - Built-int are a form of cheating; I'm not intending to
   *   implement lib functions in morpho assembly
   */

// Entry point: fib(n)
	new StoreArgVal(-1, 0, 10),
	new Call(3, -1),
	new Return(),

// Start of function fib(n)
	new StoreArgVar(-1, 0, 0),
	new StoreArgVal(-1, 1, 2),
	new Call(le, -1),
	new GoFalse(3),
	new MakeVal(1),
	new Return(),

	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 1),
	new Call(subtract, -3),
	new StoreArgAcc(-2, 0),
	new Call(3, -2),
	new StoreArgAcc(-1, 0),
	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 2),
	new Call(subtract, -3),
	new StoreArgAcc(-2, 0),
	new Call(3, -2),
	new StoreArgAcc(-1, 1),
	new Become(add, -1),
	nullptr
};

InstructionArray example(data);

#if 0
//
// Morpho2 opcodes
//
// Taken from --asm output of compiler for fibo example program.
//
Instruction *data2[] = {

//#"main[f0]" =
	new MakeVal(10),
	new Call(3 /* "#fibo[f1]" */, 1),
	new Return(),
//#"fibo[f1]" =
	new Fetch(0),
	new MakeValP(2),
	new Call(le /* "#<=[f2]" */, 2),
	new GoFalse(2 /* _0 */, 0),
	new MakeValR(1),
//_0:
	new Fetch(0),
	new MakeValP(1),
	new Call(subtract /* "#-[f2]" */, 2),
	new Call(3 /* "#fibo[f1]" */, 1),
	new FetchP(0),
	new MakeValP(2),
	new Call(subtract /* "#-[f2]" */, 2),
	new Call(3 /* "#fibo[f1]" */, 1),
	new CallR(add /* "#+[f2]" */, 2),

	nullptr
};

InstructionArray example(data);
#endif


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

			// WRI DEBUG
			IntObject *obj = dynamic_cast<IntObject *>(get_acc());
			assert(obj != nullptr);
			cout << "Fiber done; acc: "  << obj->val() << endl;
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
