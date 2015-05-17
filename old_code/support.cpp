#include "Interpreter.h"
#include "support.h"

//
// Peform a checked cast from a variable in an
// activation record to the type we want/expect.
//
template<typename T>
T *get_convert(ActivationRecord *ar, int index) {
	assert(ar != nullptr);

	AnyObject *var = ar->get(index);
	assert(var != nullptr);
	T *ret = dynamic_cast<T *>(var);
	assert(ret != nullptr);

	return ret;
}



/**
 * @brief Get variables 0 and 1 from current activation records and
 *        return them by reference as integer values.
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * Assumptions:
 *  - There are at least two variables present.
 *  - They are indeed both integers
 *
 * If the assumptions are incorrect, assertions will fire and/or
 * bad stuff will happen.
 *
 * NOTE: This function should not be externally callable!
 *
 */
void get_integers(Interpreter &interpreter, int &out_int1, int &out_int2) {
  ActivationRecord *ar = interpreter.cur_ar();

	IntObject *int1 = get_convert<IntObject>(ar, 0);
	IntObject *int2 = get_convert<IntObject>(ar, 1);

	out_int1 = int1->val();
	out_int2 = int2->val();
}


/**
 * @brief Put obj in the accumulator and return from current method.
 *
 * NOTE: This function should not be externally callable!
 *
 */
void return_acc(Interpreter &interpreter, AnyObject *obj) {
  assert(obj);

  // return value in accumulator
  interpreter.set_acc(obj);
  interpreter.ret();
}
