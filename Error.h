#ifndef ERROR_H
#define ERROR_H
#include <exception>
#include <string>

class Error : public std::exception {
private:
   std::string s;

public:
   Error() {}
   Error(std::string ss) : s(ss) {}
   ~Error() throw () {}
   void msg(std::string ss) { s = ss; }

   const char* what() const throw() { return s.c_str(); }
};




#endif // ERROR_H
