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

class IntObject : public AnyObject {
private:
	int m_val{0};

public:
	IntObject(int val) : m_val(val) {}

	AnyObject *clone() { return new IntObject(m_val); }

	int val() { return m_val; }
};

class BoolObject : public AnyObject {
private:
	bool m_val{false};

public:
	BoolObject(bool val) : m_val(val) {}

	AnyObject *clone() { return new BoolObject(m_val); }

	bool val() { return m_val; }
};

#endif // ANYOBJECT_H
