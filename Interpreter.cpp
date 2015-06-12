#include "Error.h"
#include "Interpreter.h"


static Error incorrect_number_arguments_error;

// Perhaps TODO: refactor to FiberState
void Interpreter::callClosure(int narg) {
	Closure *c = get_arg(narg).cast<Closure>();
	assert(c != nullptr);

	if(narg != c->narg()) {
		std::ostringstream buf;

		buf << "Incorrect number of arguments to closure call. "
				<< "Should be " << c->narg() << " but is " << narg << ".";

		incorrect_number_arguments_error.msg(buf.str());

		throw incorrect_number_arguments_error;
	}

	if( c->call_builtin(*this)) {
		return;
	}

	push_return_continuation(narg + 1);

	m_pc   = c->getPC();
	m_pc--;  //Adjust for pc increment.
	m_code = c->code();
	fixStackForClosureCall(get_acc(), c->stack(),c->narg());
}
