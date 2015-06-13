#include <cassert>
#include <iostream>
#include "Error.h"
#include "Loader.h"

using namespace std;

Error invalid_file("Invalid Morpho executable file");


Loader::Loader() {
}


unsigned Loader::read_uint(std::ifstream &file, int size) {
  unsigned out = 0;
  unsigned char tmp[1];

  for (int i = 0; i < size; ++ i) {
    file.read((char *) tmp, 1);

    //cout << "tmp: " << (unsigned) tmp[0] << endl;

    out <<= 8;
    out += (unsigned) tmp[0];
    //cout << "out: " << out << endl;
  }

  //cout << "out final: " << out << endl;
  return out;
}

string Loader::read_string(std::ifstream &file) {
  string out;
  unsigned size = read_uint(file, 2);
  assert(size > 0);

  char *tmp = new char[size + 1];

  file.read((char *) tmp, size);
  tmp[size] = '\0';

  out = tmp;

  delete [] tmp;

  return out;
}


char Loader::read_char(std::ifstream &file) {
  char out;

  file.read(&out, 1);

  return out;
}


void Loader::read_operation(std::ifstream &file) {
	string name = read_string(file);

	cout << "op: " << name << endl;
	/*
	Operation op = Operations.createOp(name);
	int n=op.argCount();
	for( int i=0 ; i!=n ; i++ )
		op.setArg(i,loadObject());
	return op;
	*/
}




void Loader::load(std::string filename) {
  ifstream file (filename, ios::in|ios::binary);
  if (file.is_open()) {
    unsigned magic = read_uint(file, 4);
    //cout << "magic: " << magic << endl;
    if (magic != MAGIC + 1) {
      throw invalid_file;
    }

    string filetype = read_string(file);
    if (filetype != "Morpho") {
      throw invalid_file;
    }

    int major_version = read_uint(file, 4);
    int minor_version = read_uint(file, 4);
    int build_version = read_uint(file, 4);

    cout << "Type: " << filetype << ", "
         << "v. " << major_version << "." << minor_version
         << ", build: " << build_version << endl;

		unsigned pc   = read_uint(file, 4);
		unsigned size = read_uint(file, 4);

		cout << "pc: " << pc << ", size: " << size << endl;

		//code = new Operation[n];
		for (unsigned i = 0; i != size; i++ ) {
			char typecode = read_char(file);
			if( typecode == -128 ) {
				/* code[i] = */ read_operation(file);
			}	else {
				cout << "int: " << read_uint(file, 4) << endl;
				/* code[i] =  code[loader.loadInt()]; */

			}

			if (i == 10) break;
		}


    file.close();
  }
  else {
    cout << "Unable to open file";
  }
}
