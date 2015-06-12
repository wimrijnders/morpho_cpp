//
// Opcodes from examples
//
// Taken from --asm output of compiler for example programs.
//
// The generated opcodes depend on the compiler version
///////////////////////////////////////////////////////////
#include "../operation/Operations.h"
#include "../builtins.h"

using namespace operation;

//
// Opcodes for fibo.morpho
//
// NOTE: This is from a compiler version preceding current
//       morpho and morpho2. The opcodes are valid in all versions
//
Operation *data_fibo_old_morpho[] = {

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
