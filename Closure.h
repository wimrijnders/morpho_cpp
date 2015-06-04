#ifndef CLOSURE_H
#define CLOSURE_H
#include "CallContext.h"
#include "common/AnyObject.h"
#include "RunContext.h"

/**
 * @brief Direct translation of java version.
 *
 * Not sure if nargs, nenv needed, keeping them in for now.
 */
class Closure : public CallContext, public AnyObject, public RunContext {
public:

  Closure(lib_func &func, const Stack &env, int nenv, int narg) :
    CallContext(func, narg, nenv)
  {
    set_stack(env);
  }


  Closure(int pc, const Stack &env, int nenv, int narg, OperationArray *code) :
    CallContext(0, narg, nenv)  // NOTE: first param 'offset' not used
  {
    m_pc = pc;
    set_stack(env);
    m_code = code;
  }


  Closure(int pc, int narg, OperationArray *code) :
    CallContext(0, narg, 0)  // NOTE: first param 'offset' not used
  {
    m_pc = pc;
    // Stack remains unset here
    m_code = code;
  }

  AnyObject *clone() override {
    return new Closure(*this);
  }

};

#endif // CLOSURE_H
