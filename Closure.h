#ifndef CLOSURE_H
#define CLOSURE_H
#include "common/AnyObject.h"
#include "RunContext.h"

/**
 * @brief Direct translation of java version.
 *
 * Not sure if nargs, nenv needed, keeping them in for now.
 */
class Closure : public AnyObject, public RunContext {
private:
  int m_nenv{-1};
  int m_narg{-1};

public:
  Closure(int pc, const StackLink &env, int nenv, int narg, OperationArray * code) {
    m_pc = pc;
    set_stack(env);
    m_nenv = nenv;
    m_narg = narg;
    m_code = code;
  }

  Closure(int pc, int narg, OperationArray * code) {
    m_pc = pc;
    // Stack remains unset here
    m_nenv = 0;
    m_narg = narg;
    m_code = code;
  }

  AnyObject *clone() override {
    return new Closure(*this);
  }
};

#endif // CLOSURE_H
