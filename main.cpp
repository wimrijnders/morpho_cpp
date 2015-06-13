#include <cassert>
//#include <atomic>
#include <string>
//#include <thread>
#include <iostream>
#include <vector>
#include "common/Operation.h"
#include "Stack.h"
#include "Continuation.h"
#include "operation/Operations.h"
#include "builtins.h"
#include "Loader.h"
#include "Machine.h"

using std::cout;
using std::endl;
using std::vector;
using std::string;
using namespace operation;


#include "test_code/closure.cpp"
#include "test_code/fibertest.cpp"


OperationArray example(data_closure_morpho2);


//////////////////////////
// Entry point
//////////////////////////

int main(int argc, char *argv[]) {
	Loader loader;
	loader.load("/home/wim/projects/try/morpho/morphodist/closures.out");
	return 0;

	Machine machine;

	// load program here
	machine.add_code(&example);

	machine.start();

	return 0;
}


