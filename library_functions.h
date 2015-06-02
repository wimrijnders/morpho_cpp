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
// These function don't use continuations in the calls; therefore and
// official return with popping continuations should not be performed.
//
// For now, any operations handling numeric values deals with
// integers only.
//
///////////////////////////////////////////////////////////////////////

class Interpreter;

/**
 * @brief Put obj in the accumulator and return from current method.
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * NOTE: This function should not be externally callable!
 *
 */
#if 0
void return_acc(Interpreter &interpreter, AnyObject *obj) {
  assert(obj);

  // return value in accumulator
  interpreter.set_acc(obj);
  interpreter.ret();
}
#endif

void set_acc(Interpreter &interpreter, int val);

void le(Interpreter &interpreter);
void subtract(Interpreter &interpreter);
void add(Interpreter &interpreter);


#endif //LIBRARY_FUNCTIONS_H
