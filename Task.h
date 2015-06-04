#ifndef TASK_H
#define TASK_H

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
};


class Task: public Runnable, public RunnableList<Fiber> {
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


#endif // TASK_H
