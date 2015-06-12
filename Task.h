#ifndef TASK_H
#define TASK_H
#include <iostream>
#include "common/Runnable.h"
#include "Interpreter.h"

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
		using std::cout;
		using std::endl;

		loop();

		if (is_dead()) {
			set_done();

			// WRI DEBUG
			cout << "Fiber done; acc: "  << get_acc()->dump() << endl;
		}
	}

public:
	Fiber(OperationArray *program) :
		Interpreter(program)
	{
		assert(program != nullptr);
		assert(program->size() > 0);

		m_state = READY;
	}

	Fiber(const FiberState &fs) :
	 Interpreter(fs)
	{}
};


class Task: public Runnable, public RunnableList<Fiber> {
private:
	Fiber *m_running_fiber{nullptr};  // Only set when executing a fiber

	virtual bool execute_intern(Fiber &fiber) override {
		m_running_fiber = &fiber;
		fiber.run();
		m_running_fiber = nullptr;
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

	ObjectRef pop() {
		assert(m_running_fiber != nullptr);

		return m_running_fiber->pop();
	}

	ObjectRef get_acc() {
		assert(m_running_fiber != nullptr);

		return m_running_fiber->get_acc();
	}

	void killFiber() {
		assert(m_running_fiber != nullptr);

		return m_running_fiber->set_done();
	}
};


#endif // TASK_H
