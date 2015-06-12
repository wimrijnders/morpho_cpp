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
	Machine machine;

	// load program here
	machine.add_code(&example);

	machine.start();

	return 0;
}


