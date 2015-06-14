/**
 * Library definitions.
 *
 * These are called directly, not bothering with the morpho assembly code syntax here
 */
#ifndef LIBRARY_FUNCTIONS_H
#define LIBRARY_FUNCTIONS_H
#include <map>
#include <string>

extern std::map<int, std::string> builtins;



///////////////////////////////////////////////////////////////////////
// Built-in functions.
//
// These function don't use continuations in the calls; therefore and
// official return with popping continuations should not be performed.
//
// For now, any operations handling numeric values deals with
// integers only.
//
///////////////////////////////////////////////////////////////////////

class Interpreter;
class Task;


void set_acc(Interpreter &interpreter, int val);

void le(Interpreter &interpreter);
void eq(Interpreter &interpreter);
void ne(Interpreter &interpreter);
void subtract(Interpreter &interpreter);
void inc(Interpreter &interpreter);
void dec(Interpreter &interpreter);
void add(Interpreter &interpreter);
void makeArray(Interpreter &interpreter);
void array_get(Interpreter &interpreter);
void array_put(Interpreter &interpreter);
void write(Interpreter &interpreter);
void writeln(Interpreter &interpreter);

void makeChannel(Interpreter &interpreter);
void channelWrite(Interpreter &interpreter);
void killFiber(Interpreter &interpreter);


#endif //LIBRARY_FUNCTIONS_H
