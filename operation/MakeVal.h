#ifndef MAKEVAL_H
#define MAKEVAL_H
#include "../common/AnyObject.h"

namespace operation {

class MakeVal: public Operation {
private:
	ObjectRef m_val;

public:
	explicit MakeVal(int val) {
		m_val.reset(new IntObject(val));
	}

	explicit MakeVal(bool val) {
		m_val.reset(new BoolObject(val));
	}


	/**
	 * @brief Explicit nullptr parameter to avoid ambiguity
	 */
	explicit MakeVal(std::nullptr_t val = nullptr) {
		// Nothing to do...
	}

	explicit MakeVal(const char *val) {
		m_val.reset(new StringObject(val));
	}

	void execute(Interpreter &interpreter) override {
		interpreter.set_acc(m_val);
	}
};

using MakeValP = OperationP<MakeVal>;
using MakeValR = OperationR<MakeVal>;


} // namespace operation


#endif // MAKEVAL_H
