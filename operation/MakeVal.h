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

	/**
	 * @brief Explicit nullptr parameter to avoid ambiguity
	 */
	explicit MakeVal(std::nullptr_t val = nullptr) :
		m_val(val)
	{}

	explicit MakeVal(AnyObject *val) :
		m_val(val)
	{}

	explicit MakeVal(const char *val) :
		m_val(new StringObject(val))
	{}

	void execute(Interpreter &interpreter) override {
		interpreter.set_acc(m_val);
	}
};

using MakeValP = OperationP<MakeVal>;
using MakeValR = OperationR<MakeVal>;


} // namespace operation


#endif // MAKEVAL_H
