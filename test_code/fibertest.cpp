// This version is actually the task test.
// Need to (un)comment code to make it the fiber test.

Operation *data_tasktest_morpho2[] = {

//"fibertest.mexe" = fiberTest in
//{{
//0 /* #"fiberTest[f0]" */ =
//[
  new MakeVal(nullptr),
  new MakeValP(500000),
  new CallP(makeChannel /* #"makeChannel[f0]" */, 0),
  new MakeClosureP(1 /* _0 */, 0, 3, 5 /* _1 */),
//_0:
  new Fetch(1),
  new Call(dec /* #"dec[f1]" */, 1),
  new Store(1),
  new CallR(killFiber /* #"[f0]" */, 0),
//_1:
  new MakeClosureP(1 /* _2 */, 0, 4, 0 /* _3 */),
//_2:
  new Fetch(2),
  new MakeValP(nullptr),
  new Call(0 /* #"<-[f2]" */, 2),
  new CallR(0 /* #"killTask[f0]" */, 0),
//_3:
  new MakeValP("Creating "),
  new FetchP(1),
  new Call(0 /* #"++[f2]" */, 2),
  new MakeValP(" fibers..."),
  new Call(0 /* #"++[f2]" */, 2),
  new Call(write /* #"writeln[f1]" */, 1),
  new MakeVal(0),
  new Store(0),
//_4:
  new Fetch(0),
  new FetchP(1),
  new Call(ne /* #"!=[f2]" */, 2),
  new GoFalse(8 /* _5 */, 0),
// Fiber version
//  new Fetch(3),
//  new Call(0 /* #"startFiber[f1]" */, 1),
// Task version
  new Fetch(4),
  new Call(0 /* #"startTask[f1]" */, 1),
// end Task version
//_6:
  new Fetch(0),
  new MakeValP(1),
  new Call(add /* #"+[f2]" */, 2),
  new Store(0),
  new Go(10 /* _4 */),
//_5:
  new MakeVal("All "),
  new FetchP(1),
  new Call(0 /* #"++[f2]" */, 2),
  new MakeValP(" fibers have been created."),
  new Call(0 /* #"++[f2]" */, 2),
  new Call(0 /* #"writeln[f1]" */, 1),
  new MakeVal("Running all the "),
  new FetchP(1),
  new Call(0 /* #"++[f2]" */, 2),
  new MakeValP(" fibers..."),
  new Call(0 /* #"++[f2]" */, 2),
  new Call(write /* #"writeln[f1]" */, 1),
//_7:
  new Fetch(1),
  new MakeValP(0),
  new Call(ne /* #"!=[f2]" */, 2),
  new GoFalse(7 /* _8 */, 0),
// Extra fiber:
//  new Call(0 /* #"yieldFiber[f0]" */, 0),
  new Fetch(2),
  new Call(0 /* #"<-[f1]" */, 1),
  new Fetch(1),
  new Call(dec /* #"dec[f1]" */, 1),
  new Store(1),
  new Go(9 /* _7 */),
//_8:
  new MakeVal("Done, it works!"),
  new CallR(write /* #"writeln[f1]" */, 1),
//];
//}}
//*
//BASIS
//;

	nullptr
};
