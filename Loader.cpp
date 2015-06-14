#include <cassert>
#include <iterator>   // distance()
#include <algorithm>  // sort(), unique()
#include <iostream>
#include <sstream>
#include "Error.h"
#include "builtins.h"
#include "Loader.h"

using namespace std;

Error invalid_file("Invalid Morpho executable file");
Error unknown_typecode("Unknown typecode encountered on load");
Error unknown_builtin("Call to unknown builtin encountered on load");


static string convertDouble(double value) {
  std::ostringstream o;
  if (!(o << value))
    return "";
  return o.str();
}

static string convertInt(int value) {
  std::ostringstream o;
  if (!(o << value))
    return "";
  return o.str();
}


void Loader::CodeDump::copy(int val) {
  auto &item = m_list.at(val);
  ++item.m_copy_count;

  auto tmp = item;
  tmp.m_copy_count  = 0;
  tmp.m_copied_from = val;

  m_list.push_back(tmp);
}


void Loader::CodeDump::display() {
  // Determine jump labels
  vector<int> labels;

  int count = 0;
  for (auto &line : m_list) {
    if (line.m_has_offset) {
      labels.push_back(count + line.m_offset);
    }
    ++count;
  }
  std::sort(labels.begin(), labels.end());
  auto it = std::unique(labels.begin(), labels.end());
  labels.resize(std::distance(labels.begin(),it));

  // Remove negative lines counts, which appear for obscure reasons
  // These screw up the output.
  while (labels.at(0) < 0) {
    labels.erase(labels.begin());
  }

/*
  cout << "Labels:" << endl;
  for (auto line : labels) {
    cout << "  " << line << endl;
  }
*/

  // Determine max label size
  unsigned name_size = 0;
  for (auto &line : m_list) {
    if (name_size < line.m_line.length()) {
      name_size = line.m_line.length();
    }
  }



  count = 0;
  for (auto &line : m_list) {
    // Insert labels where necessary
    if ( count == labels.at(0)) {
      cout << "_" << count << ":" << endl;
      labels.erase(labels.begin());
    }

    cout /* << count << ": "  */
         << "   "
         << line.display(count, name_size);


    cout << endl;
    ++count;
  }
}


void Loader::CodeDump::DumpItem::offset(int val) {
  m_has_offset = true;

  // PC increment needs to be taken into account
  m_offset = val + 1;
}


std::string Loader::CodeDump::DumpItem::display(unsigned op_count, unsigned name_size) {
	std::ostringstream buf;

	auto pad = [&cout, &name_size] (const string &name) -> string {
		ostringstream buf;

		unsigned pad = 0;
		if (name.length() < name_size) {
			pad = name_size - name.length();
		}

		for (unsigned i = 0; i < pad; ++i) {
			buf << " ";
		}

		buf << " ";

		return buf.str();
	};


  buf << m_line << pad(m_line);

  bool did_first = false;
  for (auto &param : m_params) {
    if (did_first) {
      buf << ", ";

    } else {
      did_first = true;

      // The assumption here is that code jumps are in the first parameter only.
      // Until now, this has been true.
      if (m_has_offset) {
        buf << "_" << (m_offset + op_count);
        continue;
      }
    }

    buf << param;
  }

  bool has_comment =   /* m_has_offset || */ m_copy_count > 0 || m_copied_from != -1;

  if (has_comment) {
    name_size = 50;  // hackish
    buf << pad(buf.str()) << ";";
  }

  //if (m_has_offset) {
  //  buf << " offset: " << m_offset;
  //}

  if (m_copy_count > 0) {
    buf << " op: #" << op_count <<", copied: " << m_copy_count;
  }

  if (m_copied_from != -1) {
    buf << " from: #" << m_copied_from;
  }

  return buf.str();
}


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

  std::ostringstream buf;
  CodeDump::DumpItem dump_item;

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

	buf /* << file.tellg() << ":  " */
			<< name << " ";

	for (int i = 0; i < pad; ++i) {
		buf << " ";
	}
	buf << " ";


	// We derive the number of arguments from the name
	if (name[name.length() - 2] == '/') {
		int num_args = name[name.length() - 1] - '0';

		//auto sep = [&buf] (int i) {
		//	if (i != 0) {
		//		buf << ", ";
		//	}
		//};

		for (int i = 0; i < num_args; ++i) {
			int tc = read_char(file);

			switch (tc) {
			case TC_INT:
			{
				unsigned val = read_uint(file, 4);

				//sep(i);
				if (i == 0 && is_call(name)) {
					if ( TOP_BUILTIN  - NUM_BUILTIN < val &&  val <= TOP_BUILTIN) {
						int index = TOP_BUILTIN - val;
						//cout << "builtin_" << (TOP_BUILTIN - val);
						try {
							//buf << "_" << builtins.at(index);
							dump_item.param( "_" + builtins.at(index));
						} catch(...) {
							cout << "FATAL: no builtin with index " << index << endl;
							throw unknown_builtin;
						}
					} else {
						//buf << "_" << (int) val;
						dump_item.param(convertInt(val));
						dump_item.offset(val);
					}
				} else {
					//buf << val;
					dump_item.param(convertInt(val));
				}
			}
				break;
			case TC_STRING:
				//sep(i);
				//buf << "\"" << read_string(file) << "\"";
				dump_item.param("\"" + read_string(file) + "\"");
				break;
			case TC_DOUBLE:
				//sep(i);
				//buf << read_double(file);
				dump_item.param(convertDouble(read_double(file)));
				break;
			case TC_TRUE:
				//sep(i);
				//buf << "true";
				dump_item.param("true");
				break;
			case TC_FALSE:
				//sep(i);
				//buf << "false";
				dump_item.param("false");
				break;
			case TC_NULL:
				//sep(i);
				//buf << "null";
				dump_item.param("null");
				break;
			default:
				cout << "Error: Unknown type code " << tc << endl;
				throw unknown_typecode;
			}

		}
	}

	dump_item.line(buf.str());
	m_code_dump.add(dump_item);
}


void Loader::load(ifstream &file) {
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
				// If you get here, the current file position is at an integer value
				// representing an absolute operation index in the current buffer.
				//
				// The operation pointed to is then used for the current operation.
				// This, of course, is a java artefact.
				//
				// TODO: Devise scheme to make this work in C++
				//
				int val = read_uint(file, 4);

				m_code_dump.copy(val);

			}

			//if (i == 100) break;
		}
}


void Loader::load(std::string filename) {
  ifstream file (filename, ios::in|ios::binary);
  if (file.is_open()) {
    load(file);

    m_code_dump.display();

    // file should be at end now
    if (!file.eof()) {
      // Apparently, inflated output is padded with zeroes

      char tmp;
      int remaining = 0;
      int remaining_zeroes = 0;
      while(!file.eof()) {
        file >> tmp;
        if (tmp == 0) ++remaining_zeroes;
        ++remaining;
      }

      cout << endl
           << "Warning: file not at eof. "
           << "Bytes remaining: " << remaining << ", zeroes: " << remaining_zeroes
           << endl;


    }

    file.close();
  }
  else {
    cout << "Unable to open file '" << filename  << "'." << endl;
  }
}
