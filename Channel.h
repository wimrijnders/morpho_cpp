#ifndef CHANNEL_H
#define CHANNEL_H
#include <deque>
#include "mutex"
#include "common/AnyObject.h"
#include "FiberState.h"
#include "Machine.h"


class Channel : public AnyObject {
  using lock = std::lock_guard<std::mutex>;

private:
  std::mutex m_mutex;
  ObjectRef  m_val;
  bool hasChangedSinceLastNotify{false};

  class Eof :public AnyObject {
  public:
    AnyObject *clone() override {
      return new Eof;
    }

  };
  static ObjectRef eof;

  bool hasValue() {
    return m_val.get() != nullptr;
  }

protected:
	/**
	 * @brief The Waiter class
	 *
	 * TODO: Figure out if members are all necessary.
	 *       They all come from a single task.
	 */
	class Waiter {
	public:
		FiberState  *m_f{nullptr};
		Task        *m_t{nullptr};
		Machine     *m_m{nullptr};

		Waiter(Task &t)	{
			// TODO: m_m = interpreter.getMachine();
			m_t = &t;
			m_f = new FiberState((FiberState &) t);
		}

		void makeReady() {
			// Implementation of Task::makeReady()
			Fiber f(*m_f);
			m_t->add(f);

			// TODO
			//m_m->makeReady(m_t);
		}
	};

	class queue : public std::deque<Waiter *>  {
	public:
		Waiter *pop() {
			Waiter *tmp = front();
			front() = nullptr;
			pop_front();
			return tmp;
		}
	};

	queue m_readers;
	queue m_writers;

	void kickQueues() {
		Waiter *r = nullptr;
		Waiter *w = nullptr;

		while (true)	{
			lock lck(m_mutex);

			if(m_val == eof) {
					// The channel is closed, but there may be readers and writers
					// TODO: notifyAll();
					if(!m_readers.empty()) {
						r = m_readers.pop();
						r->m_f->set_acc(eof);
					}

					if(!m_writers.empty()) {
						w = m_writers.pop();
					}

					if(r == nullptr && w == nullptr) {
						return;
					}
				}
				else if(hasValue()) {
					// The channel is open and currently has a value
					if(m_readers.empty()) return;
					r = m_readers.pop();
					r->m_f->set_acc(m_val);
					m_val.reset();
					hasChangedSinceLastNotify = true;
					//TODO: notifyAll();
				}	else {
					// The channel is open and currently has no value
					if(m_writers.empty()) return;
					w = m_writers.pop();
					m_val = w->m_f->get_acc();
					hasChangedSinceLastNotify = true;
					//TODO: notifyAll();
				}

			if(r != nullptr) {
				r->makeReady();
				delete r;
			}

			if(w != nullptr) {
				w->makeReady();
				delete w;
			}
		}
	}

	void kickSubscribers() {
/*	TODO: Is this necessary?

		lock lck(m_mutex);

		ChannelSubscriber *r = nullptr;
		ChannelSubscriber *w = nullptr;

		if(!hasChangedSinceLastNotify) return;
		if(hasValue()) {
			r = readSubscriber;
			if(value == eof) {
				w = writeSubscriber;
				readSubscriber = writeSubscriber = nullptr;
			}
		}	else {
			w = writeSubscriber;
		}
		hasChangedSinceLastNotify = false;

		if(r != nullptr) r.notifyReadable(this);
		if(w != nullptr) w.notifyReadable(this);
*/
	}

public:
	Channel(const Channel &rhs){
		// Workaround to init static member inline
		if (!eof) {
			eof = new Eof;
		}

		m_val = rhs.m_val;
		hasChangedSinceLastNotify = hasValue();
	}


  Channel() {
    // Workaround to init static member inline
    if (!eof) {
      eof = new Eof;
    }
  }

  AnyObject *clone() override {
    return new Channel(*this);
  }

	bool tryToWrite(ObjectRef val)	{
		lock lck(m_mutex);

		if(m_val == eof) return true;
		if(hasValue()) return false;
		m_val = val;
		hasChangedSinceLastNotify = true;

		// TODO: restart all waiting threads (following is java construct
		//notifyAll();

		return true;
	}

	void kickReadersAndWriters() {
		kickQueues();
		kickSubscribers();
	}

	void addWriter(Task &t) {
		lock lck(m_mutex);

		m_writers.push_back(new Waiter(t));
		kickReadersAndWriters();
	}

};

#endif // CHANNEL_H
