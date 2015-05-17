#ifndef OLD_SUPPORT_H
#define OLD_SUPPORT_H
#include <vector>

class ActivationRecord;
class Interpreter;
class AnyObject;

void get_integers(Interpreter &interpreter, int &out_int1, int &out_int2);
void return_acc(Interpreter &interpreter, AnyObject *obj);




#endif // OLD_SUPPORT_H
