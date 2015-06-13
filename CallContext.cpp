#include "Interpreter.h"
#include "CallContext.h"


bool CallContext::call_task_func(){
	assert(m_offset == 0);
	assert(m_func == nullptr);

	if (m_task_func == nullptr) {
		return false;
	}

	//TODO m_task_func();
	return true;
}


bool CallContext::call_builtin(Interpreter &interpreter){
	assert(m_offset == 0);
	assert(m_task_func == nullptr);

	if (m_func == nullptr) {
		return false;
	}

	assert(m_offset == 0);
	m_func(interpreter);
	return true;
}


void CallContext::call(Interpreter &interpreter) {
	if (call_task_func()) {
		return;
	}
	if (call_builtin(interpreter)) {
		return;
	}

	assert(m_offset != 0);
	interpreter.call(m_offset, m_nenv, m_nargs);
}


void CallContext::become(Interpreter &interpreter) {
	if (call_builtin(interpreter)) {

		// NOTE: in the java code, the acc it temporarily stored
		//       before executing the return. At this time, this
		//       does not appear to be necessary.
		interpreter.ret();
		return;
	}

	assert(m_offset != 0);
	interpreter.become(m_offset, m_nenv, m_nargs);
}


