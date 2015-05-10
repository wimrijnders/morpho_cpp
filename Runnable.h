#ifndef RUNNABLE_H
#define RUNNABLE_H

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
				m_ready[0].clear();
				pop(m_ready);
			}

			execute();

			if (m_ready.empty() && m_running.empty()) {
				break;
			}
		}
	}


public:
	void add(const RunnableItem &item) {
		m_ready.push_back(item);
	}

	void clear() {
		for(auto &item : m_ready) item.clear();
		for(auto &item : m_running) item.clear();

		m_ready.clear();
		m_running.clear();
	}
};



#endif // RUNNABLE_H
