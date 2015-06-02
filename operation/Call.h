#ifndef CALL_H
#define CALL_H
#include "../CallContext.h"


namespace operation {


/**
 * @brief Perform a call to either a built-in function or
 *        a position in the current operations array.
 *
 */
class Call: public CallContext, public Operation {

public:
	using CallContext::CallContext;

	void execute(Interpreter &interpreter) override {
		call(interpreter);
	}
};


class CallR: public CallContext, public Operation {
public:
	using CallContext::CallContext;

	void execute(Interpreter &interpreter) override {
		become(interpreter);
	}
};

}  // namespace operation

#endif // CALL_H
