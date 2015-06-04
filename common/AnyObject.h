#ifndef ANYOBJECT_H
#define ANYOBJECT_H
#include <typeinfo> // typeid(obj).name()
#include <cxxabi.h> // abi::__cxa_demangle
#include <string>
#include <sstream>

/**
 * Proxy for later handling.
 */
class AnyObject {

protected:
  virtual void to_str(std::ostream &os) {
    // default does nothing special
    os << "<value not displayed>";
  }

public:
	virtual ~AnyObject() {}

	// TODO: Following should be const call
	virtual AnyObject *clone() = 0;

	std::string dump() {
		std::ostringstream buf;

		char *realname = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);

		buf << realname << ": ";
		to_str(buf);

		return buf.str();
	}
};


class ObjectRef : public std::shared_ptr<AnyObject> {
public:
	ObjectRef()	{}


	ObjectRef &operator =(AnyObject *rhs)	{
		reset(rhs);
		return *this;
	}


	/**
	 * @brief Access contained pointer while casting to correct type.
	 *
	 * This is a convenience function to avoid having to use dynamic_cast all the time.
	 *
	 * **DON'T EVER DELETE THE PASSED POINTER!**. Let shared_ptr handle it.
	 */
	template<typename T>
	T *cast() {
		auto val = dynamic_cast<T *>(get());
		assert(val != nullptr);

		return val;
	}

#if 0
	//Tryout for retrieving contained value from an AnyObject instance.
	//So close; it doesn't work because there is no default ctor for classes
	//derived from AnyObject. And I'm not sure if I want default ctor's.

	template<typename T>
	decltype(T().m_val) val() {
		auto obj = cast<T>();
		assert(obj != nullptr);

		return obj->val();
	}
#endif
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


	void clear() {
		m_array.clear();
	}


	AnyObject *clone() override {
		return new ArrayObject(*this);
	}


	ObjectRef &get(int index) {
		return m_array[index]; //.clone();
	}


	void set(int index, ObjectRef &val) {
		m_array[index] = val;
	}
};


class IntObject : public AnyObject {
private:
	int m_val{0};

protected:
  void to_str(std::ostream &os) override {
    os << m_val;
  }

public:
	IntObject(int val) : m_val(val) {}

	AnyObject *clone() { return new IntObject(m_val); }

	int val() { return m_val; }
};


class BoolObject : public AnyObject {
private:
	bool m_val{false};

protected:
  void to_str(std::ostream &os) override {
    os << m_val;
  }

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
