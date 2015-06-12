#include <iostream>
#include "builtins.h"
#include "Channel.h"
#include "Interpreter.h"


static void get_integer(Interpreter &interpreter, int &out_int) {
  auto val = dynamic_cast<IntObject *>(interpreter.get_acc().get());

  assert(val != nullptr);

  out_int = val->val();
}


static void pop_integer(Interpreter &interpreter, int &out_int) {
  auto obj = interpreter.pop();
  auto val = obj.cast<IntObject>();

  assert(val != nullptr);

  out_int = val->val();
}


/**
 * @brief Compare first item on stack with acc.
 *
 * The stack value is popped. The comparison is:
 *
 *     stack value <= acc value
 *
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * Assumptions:
 *  - There is at least one variable on the stack,
 *  - and it's an integer
 *  - They acc contains an integer value
 *
 * If the assumptions are incorrect, assertions will fire and/or
 * bad stuff will happen.
 *
 * NOTE: This function should not be externally callable!
 *
 */
static void get_integers(Interpreter &interpreter, int &out_int1, int &out_int2) {
  pop_integer(interpreter, out_int1);
  get_integer(interpreter, out_int2);
}


/**
 * @brief Put integer val in the accumulator.
 *
 * This is a (perhaps) temporary convenience function to facilitate
 * dealing with operations handling two integer values.
 *
 * NOTE: This function should not be externally callable!
 *
 */
void set_acc(Interpreter &interpreter, int val) {
  interpreter.set_acc(new IntObject(val));
}


void le(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	interpreter.set_acc(new BoolObject(int1 <= int2));
}

void eq(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	interpreter.set_acc(new BoolObject(int1 == int2));
}

void ne(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	interpreter.set_acc(new BoolObject(int1 != int2));
}

void subtract(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	set_acc(interpreter, int1 - int2);
}


void inc(Interpreter &interpreter) {
	int int1;
	get_integer(interpreter, int1);

	set_acc(interpreter, int1 + 1);
}

void dec(Interpreter &interpreter) {
	int int1;
	get_integer(interpreter, int1);

	set_acc(interpreter, int1 - 1);
}


void add(Interpreter &interpreter) {
	int int1, int2;
	get_integers(interpreter, int1, int2);

	set_acc(interpreter, int1 + int2);
}


void makeArray(Interpreter &interpreter) {
	int size;
	get_integer(interpreter, size);
	interpreter.set_acc(new ArrayObject(size));
}


void array_get(Interpreter &interpreter) {
  int index;
  get_integer(interpreter, index);
  // TODO: throw new Error("Invalid index type for builtin JAVA_ARRAYGET");

  auto obj = interpreter.pop();
  auto arr = obj.cast<ArrayObject>();

  interpreter.set_acc(arr->get(index));
}


void array_put(Interpreter &interpreter) {
  int index;
  pop_integer(interpreter, index);
  // TODO: throw new Error("Invalid index type for builtin JAVA_ARRAYGET");

  auto obj = interpreter.pop();
  auto arr = obj.cast<ArrayObject>();

  arr->set(index, interpreter.get_acc());

  // Acc stays same value
}

void writeln(Interpreter &interpreter) {
  auto val = interpreter.get_acc().cast<StringObject>();

  std::cout << val->val() << std::endl;
}


///////////////////////////////////////////
// Channels
///////////////////////////////////////////

void makeChannel(Interpreter &interpreter) {
	interpreter.set_acc(new Channel);
}


/**
 * @brief channelWrite
 *
 * TODO: All the other built-ins take an Interpreter instance
 *       as parameter. Make this uniform.
 */
void channelWrite(Task &t) {
	ObjectRef channel = t.pop();
	auto c  = channel.cast<Channel>();

	// acc contains value to write
	ObjectRef val = t.get_acc();

	if(c->tryToWrite(val)) {
		c->kickReadersAndWriters();
		return;
	}
	c->addWriter(t);
	//TODO: interpreter.exceptional = KILLFIBER;
}


void killFiber(Interpreter &interpreter) {
	// TODO: UGLY! Fix this.
	Task *t = static_cast<Task *>(&interpreter);
	assert(t != nullptr);

	t->killFiber();
}
