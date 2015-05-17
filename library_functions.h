/**
 * Library definitions.
 *
 * These are called directly, not bothering with the morpho assembly code syntax here
 */
#ifndef LIBRARY_FUNCTIONS_H
#define LIBRARY_FUNCTIONS_H

///////////////////////////////////////////////////////////////////////
// Built-in functions.
//
// These are treated as actual functions in the current assembly code,
// in these sense that an activation record is created for each call
// which must also be removed upon return.
//
// For now, any operations handling numeric values deals with
// integers only.
//
///////////////////////////////////////////////////////////////////////


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
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
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

/**
 * @brief Put integer val in the accumulator and return from current method.
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * NOTE: This function should not be externally callable!
 *
 */
void return_acc(Interpreter &interpreter, int val) {
  return_acc(interpreter, new IntObject(val));
}


void le(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	return_acc(interpreter, new BoolObject(int1 <= int2));
}


void subtract(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	return_acc(interpreter, int1 - int2);
}


void add(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	return_acc(interpreter, int1 + int2);
}


#endif //LIBRARY_FUNCTIONS_H
