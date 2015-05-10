#ifndef ANYOBJECT_H
#define ANYOBJECT_H

/**
 * Proxy for later handling.
 */
class AnyObject {
public:
	virtual ~AnyObject() {}
	virtual AnyObject *clone() = 0;
};


#endif // ANYOBJECT_H
