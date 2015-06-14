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
	new MakeVal(nullptr),                   // Why would you want to create a null value?
	new MakeValP(10),                       // .. and then put it on the stack??
	new Call(makeArray /* #"makeArray[f1]" */, 1),
	new MakeValP(0),
	new Store(0),
// _0:
	new Fetch(0),
	new MakeValP(10),
	new Call(ne /* #"!=[f2]" */, 2),
	new GoFalse(12 /* _1 */, 0),
	new Fetch(0),
	new FetchP(1),
	new FetchP(0),
	new MakeClosureP(1 /* _3 */, 0, 3, 2 /* _4 */),
// _3:
	new FetchR(2),
// _4:
	new Call(array_put /* #"?[?]=?[f3]" */, 3),
	new Go(1, 1),   // ??? Original had offset 0 and pc got stuck
// _2:
	new Fetch(0),
	new Call(inc /* #"inc[f1]" */, 1),
	new Store(0),
	new Go(-14 /* _0 */),
// _1:
	new MakeVal(nullptr),
	new MakeValP(true),
	new MakeValP(true),
	new MakeValP(0),
	new Store(2),
// _5:
	new Fetch(2),
	new MakeValP(10),
	new Call(ne /* #"!=[f2]" */, 2),
	new GoFalse(22 /*_6 */, 0),
	new Fetch(1),
	new FetchP(2),
	new Call(array_get /* #"?[?][f2]" */, 2),
	new CallClosure(0),
	new FetchP(3),
	new GoFalse(4 /* _8 */),
	new Fetch(5),
	new FetchP(2),
	new Call(eq /* #"==[f2]" */, 2),
// _8:
	new Store(3),
	new Fetch(4),
	new GoFalse(4 /* _9 */),
	new Fetch(5),
	new MakeValP(9),
	new Call(eq /* #"==[f2]" */, 2),
// _9:
	new Store(4),
	new Go(1, 1),   // ??? Original had offset 0 and pc got stuck
// _7:
	new Fetch(2),
	new Call(inc /* #"inc[f1]" */, 1),
	new Store(2),
	new Go(-24 /* _5 */),
// _6:
	new Fetch(3),
	new GoFalse(3 /* _10 */, 0),
	new MakeVal("nested OK"),
	new Call(write /* #"writeln[f1]" */, 1),
// _10:
	new Fetch(4),
	new GoTrue(2 /* _11 */),
	new Return(),
// _11:
	new MakeVal("nonnested OK"),
	new CallR(write /* #"writeln[f1]" */, 1),
// ];
// }}
// *
// BASIS
// ;

	nullptr
};

