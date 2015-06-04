#ifndef STACKLINK_H
#define STACKLINK_H
#include <vector>
#include <algorithm>
#include "common/AnyObject.h"

/**
 * @brief TODO
 *
 * In the java version, the StackLink is a linked list
 * of objects.
 *
 * The list is operated as an inverted stack, for reasons
 * of efficiency. It's better to push/pop at the end.
 */
class Stack {
  using VarList = std::vector<ObjectRef>;
private:
  VarList m_variables;

public:
  Stack() = default;

  Stack(const Stack &rhs) {
    // Copy all items
    m_variables.clear();

    for (auto &it : rhs.m_variables) {
      m_variables.push_back(it);
    }
  }

  void push(ObjectRef &obj) {
    m_variables.push_back(obj);
  }

  /**
   * @brief Remove the specified number of objects
   *        from the 'top' of the stack.
   *
   * @param num Number of items to remove.
   */
  void pop(int num) {
    assert(num > 0);

    while (num > 0) {
      assert(!m_variables.empty());
      m_variables.pop_back();
      num--;
    }
  }


	/**
	 * @brief Remove and return the first item on the stack
	 */
	ObjectRef pop() {
		assert(!m_variables.empty());

		auto tmp = m_variables.back();

    m_variables.pop_back();
    return tmp;
  }


  Stack get_env(int nenv) const{
    Stack tmp;

		if (nenv > 0) {
			tmp.m_variables.insert(tmp.m_variables.end(), m_variables.begin(), m_variables.begin() + nenv);
		};

    return tmp;
  }


	/**
	 * @brief Prepares the stack for entering a called function.
	 *
	 * After the reorganization the stack will consist only
	 * of the common variables, at the bottom, and the arguments
	 * at the top.
	 *
	 * @param nenv is the number of variables that are common to
	 * the caller and the callee.  These variables, if any, are
	 * at the bottom of the stack.  If a global function is being
	 * called, there will be no such variables.
	 *
	 * @param narg is the number of parameters to the function.
	 * If there are parameters, then the accumulator will be
	 * the last parameter and the others will be on the top of
	 * the stack before reorganization.  After reorganization
	 * all the parameters will be on the top of the stack with
	 * the value of the accumulator topmost and the others below
	 * in order.
	 */
	void fixStackForCall(ObjectRef &acc, int nenv, int narg) {

		// Get bottom nenv variables in stack
		VarList vars;

		if (nenv > 0) {
			vars.insert(vars.end(), m_variables.begin(), m_variables.begin() + nenv);
		};

		// Get the parameters that we want at the 'top' of the stack.
		if( (narg -1) > 0 ) {
			vars.insert(vars.end(), m_variables.end() - (narg -1), m_variables.end());
		}

		// The acc value is the topmost value on the stack.
		if (narg  > 0) {
			vars.push_back(acc);
		}

		m_variables = vars;
	}


	void fixStackForClosureCall(ObjectRef &acc, Stack &env, int narg) {

		VarList vars;

		// Use passed StackLink instance as environment variables.
		vars.insert(vars.end(), env.m_variables.begin(), env.m_variables.end());

		// Get the parameters that we want at the 'top' of the stack.
		if( (narg -1) > 0 ) {
			vars.insert(vars.end(), m_variables.end() - (narg -1), m_variables.end());
		}

		// The acc value is the topmost value on the stack.
		if (narg  > 0) {
			vars.push_back(acc);
		}

		m_variables = vars;
	}

	/**
	 * @brief Retrieves an environment variable from the stack.
	 *
	 * This works on the environment side of the stack, not the parameter values!
	 * The environment var's are logically indexed back to front; hence, in this
	 * implementation they are in the correct order.
	 *
	 */
	ObjectRef &fetch(int pos) {
		return m_variables[pos]; //.clone();
	}


	/**
	 * @brief Put an environment variable on the stack.
	 *
	 * This is the counterpart to fetch().
	 *
	 * Apparently, regarding the logic in the generated opcodes,
	 * it is necessary to create an empty location (with null value)
	 * before you write to it.
	 *
	 */
	void store(ObjectRef &obj, int pos) {
		assert(obj.get() != nullptr); // Not sure if this is allowed to happen

		m_variables[pos] = obj;
	}


	/**
	 * @brief Retrieves a parameter variable from the stack.
	 *
	 * The variables are logically indexed back to front; hence, we
	 * need to access from the back.
	 *
	 */
	ObjectRef &get_arg(int pos) {
		assert(pos > 0); // value 0 is the accumulator

		return m_variables[m_variables.size() - pos ];
	}
};

#endif // STACKLINK_H
