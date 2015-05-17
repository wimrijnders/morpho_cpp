/**
 * Library definitions.
 *
 * These are called directly, not bothering with the morpho assembly code syntax here
 */
#ifndef LIBRARY_FUNCTIONS_H
#define LIBRARY_FUNCTIONS_H
#include <cassert>
#include "../common/AnyObject.h"
#include "support.h"

///////////////////////////////////////////////////////////////////////
// Built-in functions.
//
// These are treated as actual functions in the current assembly code,
// in the sense that an activation record is created for each call
// which must also be removed upon return.
//
// For now, any operations handling numeric values deals with
// integers only.
//
///////////////////////////////////////////////////////////////////////



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
