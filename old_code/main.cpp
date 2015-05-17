/*
#include <cassert>
#include "ARHandler.h"
#include "Interpreter.h"
#include "old_main.h"
*/

#include "library_functions.h"
#include "Operations.h"
#include "Task.h"
#include "Machine.h"

using namespace operation;

/**
 * Example program from chapter Example in second doc.
 *
 * Doing the assembly code only; we won't even bother with the actual language.
 *
 * The intention here is to implement the least possible functionality to make it
 * work, so that the VM can be tested.
 * Filling it in as we go along.
 */
Operation *data[] = {
  /**
   * NOTE:
   *
   * - Jumps and calls are only within current instruction array
   * - For built-in functions, the program counter is not used
   * - Built-int are a form of cheating; I'm not intending to
   *   implement lib functions in morpho assembly
   */

// Entry point: fib(n)
	new StoreArgVal(-1, 0, 10),
	new Call(3, -1),
	new Return(),

// Start of function fib(n)
	new StoreArgVar(-1, 0, 0),
	new StoreArgVal(-1, 1, 2),
	new Call(le, -1),
	new GoFalse(3),
	new MakeVal(1),
	new Return(),

	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 1),
	new Call(subtract, -3),
	new StoreArgAcc(-2, 0),
	new Call(3, -2),
	new StoreArgAcc(-1, 0),
	new StoreArgVar(-3, 0, 0),
	new StoreArgVal(-3, 1, 2),
	new Call(subtract, -3),
	new StoreArgAcc(-2, 0),
	new Call(3, -2),
	new StoreArgAcc(-1, 1),
	new Become(add, -1),
	nullptr
};

OperationArray example(data);


//////////////////////////
// Entry point
//////////////////////////

int main(int argc, char *argv[]) {
	Machine machine;

	// load program here
	machine.add_code(&example);

	machine.start();

	return 0;
}
