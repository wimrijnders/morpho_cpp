#ifndef FETCH_H
#define FETCH_H

namespace operation {


class Fetch: public Operation {
private:
	int m_pos{0};

public:
	Fetch(int pos) :
		m_pos(pos)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.doFetch(m_pos);
	}
};


using FetchP = OperationP<Fetch>;
using FetchR = OperationR<Fetch>;


} // namespace operation

#endif // FETCH_H
