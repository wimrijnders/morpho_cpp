//
// Opcodes from examples
//
// Taken from --asm output of compiler for example programs.
//
// The generated opcodes depend on the compiler version
///////////////////////////////////////////////////////////


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





//
// Opcodes from closures.morpho example program
//
// NOTE: this is for morpho version 1 (not morpho2)
//
Operation *data_closure_morpho1[] = {

// TODO: Fix all zero offsets!

//#"main[f0]" =
//	[
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
	new MakeClosureP(1 /* _3 */, 0, 5, 2 /* _4 */),
// _3:
	new FetchR(2),
// _4:
	new Call(0 /* #"?[?]=?[f3]" */, 3),
	new Drop(1),
// _2:
	new Fetch(0),
	new Call(0 /* #"inc[f1]" */, 1),
	new Store(0),
	new Go(0 /* _0 */),
// _1:
	new MakeVal(null),
	new MakeValP(true),
	new MakeValP(true),
	new MakeValP(0),
	new Store(2),
// _5:
	new Fetch(2),
	new MakeValP(10),
	new Call(0 /* #"!=[f2]" */, 2),
	new GoFalse(0 /* _6 */, 0),
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
	new Drop(1),
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

	nullptr
};
