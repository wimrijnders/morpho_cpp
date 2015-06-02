#ifndef MAKEVAL_H
#define MAKEVAL_H

namespace operation {

class MakeVal: public Operation {
private:
	AnyObject *m_val{nullptr};

public:
	explicit MakeVal(int val) :
		m_val(new IntObject(val))
	{}

	explicit MakeVal(AnyObject *val = nullptr) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.set_acc(m_val);
	}
};

using MakeValP = OperationP<MakeVal>;
using MakeValR = OperationR<MakeVal>;




#if 0
class MakeValP: public Operation {
private:
	AnyObject *m_val{nullptr};

public:
	MakeValP(int val) :
		m_val(new IntObject(val))
	{}

	explicit MakeValP(AnyObject *val = nullptr) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		// Following is the only difference with MakeVal
		interpreter.push();
		interpreter.set_acc(m_val);
	}
};

class MakeValR: public Operation {
private:
	int m_val;		// int only for now

public:
	MakeValR(int val) :
		m_val(val)
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.set_acc(new IntObject(m_val));
		// Following is the only difference with MakeVal
		interpreter.ret();
	}
};

#endif


} // namespace operation


#endif // MAKEVAL_H
