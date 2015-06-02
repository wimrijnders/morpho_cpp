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
// Opcodes from closures.morpho example program
//
// NOTE: This is for morpho2
//
Operation *data_closure_morpho2[] = {

// TODO: Fix all zero offsets!

// "closures.mexe" = main in
// {{
// #"main[f0]" =
// [
	new MakeVal(nullptr),
	new MakeValP(10),
	new Call(0 /* #"makeArray[f1]" */, 1),
	new MakeValP(0),
	new Store(0),
// _0:
	new Fetch(0),
	new MakeValP(10),
	new Call(0 /* #"!=[f2]" */, 2),
	new GoFalse(0 /* _1 */, 0),
	new Fetch(0),
	new FetchP(1),
	new FetchP(0),
	new MakeClosureP(0 /* _3 */, 0, 3, 0 /* _4 */),
// _3:
	new FetchR(2),
// _4:
	new Call(0 /* #"?[?]=?[f3]" */, 3),
	new Go(0, 1),
// _2:
	new Fetch(0),
	new Call(0 /* #"inc[f1]" */, 1),
	new Store(0),
	new Go(0 /* _0 */),
// _1:
	new MakeVal(nullptr),
	new MakeValP(true),
	new MakeValP(true),
	new MakeValP(0),
	new Store(2),
// _5:
	new Fetch(2),
	new MakeValP(10),
	new Call(0 /* #"!=[f2]" */, 2),
	new GoFalse(0 /*_6 */, 0),
	new Fetch(1),
	new FetchP(2),
	new Call(0 /* #"?[?][f2]" */, 2),
	new CallClosure(0),
	new FetchP(3),
	new GoFalse(0 /* _8 */),
	new Fetch(5),
	new FetchP(2),
	new Call(0 /* #"==[f2]" */, 2),
// _8:
	new Store(3),
	new Fetch(4),
	new GoFalse(0 /* _9 */),
	new Fetch(5),
	new MakeValP(9),
	new Call(0 /* #"==[f2]" */, 2),
// _9:
	new Store(4),
	new Go(0, 1),
// _7:
	new Fetch(2),
	new Call(0 /* #"inc[f1]" */, 1),
	new Store(2),
	new Go(0 /* _5 */),
// _6:
	new Fetch(3),
	new GoFalse(0 /* _10 */, 0),
	new MakeVal("nested OK"),
	new Call(0 /* #"writeln[f1]" */, 1),
// _10:
	new Fetch(4),
	new GoTrue(0 /* _11 */),
	new Return(),
// _11:
	new MakeVal("nonnested OK"),
	new CallR(0 /* #"writeln[f1]" */, 1),
// ];
// }}
// *
// BASIS
// ;

	nullptr
};



OperationArray example(data_closure_morpho2);


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


