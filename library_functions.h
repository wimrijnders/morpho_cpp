/**
 * Library definitions.
 *
 * These are called directly, not bothering with the morpho assembly code syntax here
 */
#ifndef LIBRARY_FUNCTIONS_H
#define LIBRARY_FUNCTIONS_H

// Int's only for now.
void smaller_equal_two(Interpreter &interpreter) {
	ActivationRecord *ar = interpreter.cur_ar();

	IntObject *int1 = get_convert<IntObject>(ar, 0);
	IntObject *int2 = get_convert<IntObject>(ar, 1);

	// return value in accumulator
	interpreter.set_acc(new BoolObject(int1->val() <= int2->val()));

	// Return from function
	operation::Return ret;
	ret.execute(interpreter);
}


// Int's only for now.
void subtract(Interpreter &interpreter) {
	ActivationRecord *ar = interpreter.cur_ar();

	IntObject *int1 = get_convert<IntObject>(ar, 0);
	IntObject *int2 = get_convert<IntObject>(ar, 1);

	// return value in accumulator
	interpreter.set_acc(new IntObject(int1->val() - int2->val()));

	// Return from function
	operation::Return ret;
	ret.execute(interpreter);
}


// Int's only for now.
void add(Interpreter &interpreter) {
	ActivationRecord *ar = interpreter.cur_ar();

	IntObject *int1 = get_convert<IntObject>(ar, 0);
	IntObject *int2 = get_convert<IntObject>(ar, 1);

	// return value in accumulator
	interpreter.set_acc(new IntObject(int1->val() + int2->val()));

	// Return from function
	operation::Return ret;
	ret.execute(interpreter);
}


#endif //LIBRARY_FUNCTIONS_H
