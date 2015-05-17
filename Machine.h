#ifndef MACHINE_H
#define MACHINE_H

class Thread {
private:
	std::thread m_thread;

	std::atomic<bool> m_running{false};

public:
	bool available() { return m_running == false; }

	void execute(Task &process) {
		assert(!m_running);

		// TODO: determine if there's a race condition, even with use of atomic.
		m_running = true;

		m_thread = std::thread(&Task::run, &process);

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
 * @brief Top-level virtual machine
 *
 * The VM can be regarded as a thread itself
 * In the text, it is run by the 'main' thread, and can execute
 * processes if multi-processing not required.
 */
class Machine: public RunnableList<Task> {
private:
	ThreadPool           m_thread_pool;

	bool execute_intern(Task &process) override {
		if (NUM_WORKER_THREADS > 0) {
			Thread *thr = m_thread_pool.get_available();
			if (thr == nullptr) return false;;

			// Run Process in context of thread
			thr->execute(process);
		} else {
			process.run();
		}

		return true;
	}

public:
	void add_code(OperationArray *code) {
		Fiber fiber(code);
		Task task;
		task.add(fiber);
		add(task);

		// Remove ptr's after copy ctor call in process
		task.clear();
		fiber.clear();
	}

	void start() {
		// Initialize worker threads here

		loop();

		m_thread_pool.join();
	}
};



#endif // MACHINE_H
