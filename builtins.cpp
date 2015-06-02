#include "library_functions.h"
#include "Interpreter.h"

/**
 * @brief Compare first item on stack with acc.
 *
 * The stack value is popped. The comparison is:
 *
 *     stack value <= acc value
 *
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * Assumptions:
 *  - There is at least one variable on the stack,
 *  - and it's an integer
 *  - They acc contains an integer value
 *
 * If the assumptions are incorrect, assertions will fire and/or
 * bad stuff will happen.
 *
 * NOTE: This function should not be externally callable!
 *
 */
static void get_integers(Interpreter &interpreter, int &out_int1, int &out_int2) {
  auto val1 = dynamic_cast<IntObject *>(interpreter.pop());
  auto val2 = dynamic_cast<IntObject *>(interpreter.get_acc());

  assert(val1 != nullptr);
  assert(val2 != nullptr);

  out_int1 = val1->val();
  out_int2 = val2->val();

  delete val1;
}

/**
 * @brief Put integer val in the accumulator.
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * NOTE: This function should not be externally callable!
 *
 */
void set_acc(Interpreter &interpreter, int val) {
  interpreter.set_acc(new IntObject(val));
}


void le(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	interpreter.set_acc(new BoolObject(int1 <= int2));
}


void subtract(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	set_acc(interpreter, int1 - int2);
}


void add(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	set_acc(interpreter, int1 + int2);
}
