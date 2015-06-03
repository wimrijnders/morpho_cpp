#ifndef ANYOBJECT_H
#define ANYOBJECT_H
#include <string>

/**
 * Proxy for later handling.
 */
class AnyObject {
public:
	virtual ~AnyObject() {}

	// TODO: Following should be const call
	virtual AnyObject *clone() = 0;
};


class ObjectRef {
private:
	AnyObject *m_val{nullptr};

public:
	ObjectRef() :m_val(nullptr)	{}

	ObjectRef(const ObjectRef &rhs)	{
		if (rhs.m_val != nullptr) {
			m_val = rhs.m_val->clone();
		}
	}

	~ObjectRef()	{
		if (m_val != nullptr) {
			delete m_val;
		}
	}


	/**
	 * @brief Empty without deleting ref'd item
	 */
	void clear() {
		m_val = nullptr;
	}


  ObjectRef &operator =(const ObjectRef &rhs)	{
    if (this == &rhs) return *this;

    if (m_val != nullptr) {
      delete m_val;
      m_val = nullptr;
    }

		if (rhs.m_val != nullptr) {
			m_val = rhs.m_val->clone();
		}

    return *this;
  }

  ObjectRef &operator =(AnyObject *rhs)	{

    if (m_val != nullptr) {
      delete m_val;
      m_val = nullptr;
    }

		if (rhs != nullptr) {
			m_val = rhs->clone();
		}

    return *this;
  }


  AnyObject *clone() {
    if (m_val == nullptr) {
      return nullptr;
    }

    return m_val->clone();
  }

};


class ArrayObject : public AnyObject {
private:
	std::vector<ObjectRef> m_array;

public:
	ArrayObject(const ArrayObject &rhs){
		for (auto &it : rhs.m_array) {
			m_array.push_back(it);
		}
	}


	ArrayObject(int size) : m_array(size) {}

	virtual ~ArrayObject() {
		// Ref's should clean themselves
	}

	/**
	 * @brief Empty array without deleting ref'd items
	 */
	void clear() {
		for (auto &it : m_array) {
			it.clear();
		}
	}


	AnyObject *clone() override {
		return new ArrayObject(*this);
	}

	AnyObject *get(int index) {
		return m_array[index].clone();
	}

	void set(int index, AnyObject *val) {
		m_array[index] = val;
	}
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


class StringObject : public AnyObject {
private:
	std::string m_val;

public:
	StringObject(std::string val) : m_val(val) {}

	AnyObject *clone() { return new StringObject(m_val); }

	const std::string &val() { return m_val; }
};
#endif // ANYOBJECT_H
