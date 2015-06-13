#include <cassert>
#include <iostream>
#include "Error.h"
#include "builtins.h"
#include "Loader.h"

using namespace std;

Error invalid_file("Invalid Morpho executable file");
Error unknown_typecode("Unknown typecode encountered on load");
Error unknown_builtin("Call to unknown builtin encountered on load");

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


double Loader::read_double(std::ifstream &file) {
  char out[8];

  // Here's hoping that the float layout corresponds to a
  // C++ float
  file.read(out, 8);

  return *((double *) out);
}



void Loader::read_operation(std::ifstream &file) {
  const unsigned MIN_SIZE    = 10;
  const unsigned TOP_BUILTIN = 3294967296;
  const unsigned NUM_BUILTIN = 113;

  auto has_prefix = [] (string &argument, string prefix) -> bool {
    return (argument.substr(0, prefix.size()) == prefix);
  };

  auto is_call = [&has_prefix] (string &argument) -> bool {
    return has_prefix(argument, "Call") || has_prefix(argument, "Go");
  };

  string name = read_string(file);

	int pad = 0;
	if (name.length() < MIN_SIZE) {
		pad = MIN_SIZE - name.length();
	}

	cout << file.tellg() << ":  " << name << " ";
	for (int i = 0; i < pad; ++i) {
		cout << " ";
	}
	cout << " ";


	// We derive the number of arguments from the name
	if (name[name.length() - 2] == '/') {
		int num_args = name[name.length() - 1] - '0';

		auto sep = [&cout] (int i) {
			if (i != 0) {
				cout << ", ";
			}
		};

		for (int i = 0; i < num_args; ++i) {
			int tc = read_char(file);

			switch (tc) {
			case TC_INT:
			{
				unsigned val = read_uint(file, 4);

				sep(i);
				if (i == 0 && is_call(name)) {
					if ( TOP_BUILTIN  - NUM_BUILTIN < val &&  val <= TOP_BUILTIN) {
						int index = TOP_BUILTIN - val;
						//cout << "builtin_" << (TOP_BUILTIN - val);
						try {
							cout << "_" << builtins.at(index);
						} catch(...) {
							cout << "FATAL: no builtin with index " << index << endl;
							throw unknown_builtin;
						}
					} else {
						cout << "_" << (int) val;
					}
				} else {
					cout << val;
				}
			}
				break;
			case TC_STRING:
				sep(i);
				cout << "\"" << read_string(file) << "\"";
				break;
			case TC_DOUBLE:
				sep(i);
				cout << read_double(file);
				break;
			case TC_TRUE:
				sep(i);
				cout << "true";
				break;
			case TC_FALSE:
				sep(i);
				cout << "false";
				break;
			case TC_NULL:
				sep(i);
				cout << "null";
				break;
			default:
				cout << "Error: Unknown type code " << tc << endl;
				throw unknown_typecode;
			}

		}
	}

	cout << endl;
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

		cout << "pc: " << pc << ", size: " << size << endl << endl;

		//code = new Operation[n];
		for (unsigned i = 0; i != size; i++ ) {
			char typecode = read_char(file);
			if( typecode == TC_OPERATION ) {
				/* code[i] = */ read_operation(file);
			}	else {
				cout << "int: " << read_uint(file, 4) << endl;
				/* code[i] =  code[loader.loadInt()]; */

			}

			if (i == 100) break;
		}


    file.close();
  }
  else {
    cout << "Unable to open file";
  }
}
