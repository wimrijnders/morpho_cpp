#include <cassert>
#include <cstring>  // strcmp()
#include <string>
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

void usage() {
	const char *str =
"Usage:\n"
"\n"
"    morpho2 [-h] [-dump <filename of dump>]\n"

	;


	cout << str << endl;
}


int main(int argc, char *argv[]) {
	if (argc > 1) {
		if (strcmp(argv[1],"-h") == 0) {
			usage();
			return 0;
		} else if (strcmp(argv[1],"-dump") == 0 && argc > 2) {
			Loader loader;
			loader.load(argv[2]);
			return 0;
		} else {
			usage();
			return 0;
		}
 }

	Machine machine;

	// load program here
	machine.add_code(&example);

	machine.start();

	return 0;
}


