#include <iostream>
#include "Loader.h"

using namespace std;

Loader::Loader() {
}


unsigned Loader::read_uint32(std::ifstream &file) {
  unsigned out = 0;
  unsigned char tmp[1];

  for (int i = 0; i < 4; ++ i) {
    file.read((char *) tmp, 1);

    cout << "tmp: " << (unsigned) tmp[0] << endl;

    out <<= 8;
    out += (unsigned) tmp[0];
    cout << "out: " << out << endl;
  }

  cout << "out final: " << out << endl;
  return out;
}


void Loader::load(std::string filename) {
  ifstream file (filename, ios::in|ios::binary);
  if (file.is_open()) {
    unsigned magic = read_int32(file);

    cout << "magic: " << magic << endl;

    file.close();
  }
  else {
    cout << "Unable to open file";
  }
}
