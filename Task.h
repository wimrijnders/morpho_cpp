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
