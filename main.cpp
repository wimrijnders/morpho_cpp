#include <cassert>
#include <atomic>
#include <string>
#include <thread>
#include <iostream>
#include <vector>
#include "common/Operation.h"
#include "StackLink.h"
#include "Continuation.h"
#include "common/Runnable.h"
#include "Interpreter.h"
#include "operation/Operations.h"
#include "library_functions.h"
#include "Task.h"

// Needs to be defined before class Machine
const int NUM_WORKER_THREADS = 0;

#include "Machine.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using namespace operation;

//
// Morpho2 opcodes
//
// Taken from --asm output of compiler for fibo example program.
//
Operation *data[] = {

//#"main[f0]" =
	new MakeVal(10),
	new Call(2 /* "#fibo[f1]" */, 1),
	new Return(),
//#"fibo[f1]" =
	new Fetch(0),
	new MakeValP(2),
	new Call(le /* "#<=[f2]" */, 2),
	new GoFalse(2 /* _0 */, 0),
	new MakeValR(1),
//_0:
	new Fetch(0),
	new MakeValP(1),
	new Call(subtract /* "#-[f2]" */, 2),
	new Call(-8 /* "#fibo[f1]" */, 1),
	new FetchP(0),
	new MakeValP(2),
	new Call(subtract /* "#-[f2]" */, 2),
	new Call(-12 /* "#fibo[f1]" */, 1),
	new CallR(add /* "#+[f2]" */, 2),

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


