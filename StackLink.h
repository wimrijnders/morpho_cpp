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
class StackLink {
  using VarList = std::vector<AnyObject *>;
private:
  VarList m_variables;

public:
  StackLink() = default;

  StackLink(const StackLink &rhs) {
    // Copy all items
    m_variables.clear();

    for (AnyObject *it : rhs.m_variables) {
      m_variables.push_back(it->clone());
    }
  }

  ~StackLink() {
    // Remove all items
    for (AnyObject *it : m_variables) {
      delete it;
    }

    m_variables.clear();
  }


  /**
   * @brief Empty the stack without deleting all the items.
   *
   * The items should have been moved beforehand to another continuation
   */
  void clear() {
    m_variables.clear();
  }

  void push(AnyObject *obj) {
    m_variables.push_back(obj->clone());
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
	AnyObject *pop() {
		assert(!m_variables.empty());

		auto tmp = m_variables.back();

    m_variables.pop_back();
    return tmp;
  }


  StackLink get_env(int nenv) const{
    StackLink tmp;

		if (nenv > 0) {
			std::copy(m_variables.begin(), m_variables.begin() + nenv, tmp.m_variables.begin());
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
	void fixStackForCall(AnyObject *acc, int nenv, int narg) {

		// Get bottom nenv variables in stack
		VarList env;

		if (nenv > 0) {
			std::copy(m_variables.begin(), m_variables.begin() + nenv, env.begin());
		};

		// Get the parameters that we want at the 'top' of the stack.
		if( (narg -1) > 0 ) {
			env.insert(env.end(), m_variables.end() - (narg -1), m_variables.end());
		}

		// The acc value is the topmost value on the stack.
		if (narg  > 0) {
			env.push_back(acc->clone());
		}

		m_variables = env;
	}


	/**
	 * @brief Retrieves a variable from the stack
	 *
	 * This works on the environment side of the stack, not the temp values!
	 * The latter are handled with push/pop.
	 * The environment var's are logically indexed back to front; hence, in this
	 * implementation they are in the correct order.
	 *
	 */
	AnyObject *fetch(int pos) {
		return m_variables[pos]->clone();
		//return m_variables[m_variables.size() -1 - pos]->clone();
	}
};

#endif // STACKLINK_H
