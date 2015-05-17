#ifndef STACKLINK_H
#define STACKLINK_H
#include "vector"
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
private:
  std::vector<AnyObject *> m_variables;
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


  void push(AnyObject *obj) {
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
};

#endif // STACKLINK_H
