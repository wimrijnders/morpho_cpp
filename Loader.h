#ifndef LOADER_H
#define LOADER_H
#include <fstream>
#include <string>

/**
 * @brief Loader::Loader
 *
 * The compiled program coming from morpho(2) is a zlib-encoded format.
 * This is the same as gzip, but without the magic number in front.
 *
 * For now, we use the following command line to uncompress the mexe files:
 *
 *     printf "\x1f\x8b\x08\x00\x00\x00\x00\x00" |cat - <<input file>> |gzip -dc > <<output file>>
 *
 * ...and then read in the generated file.
 *
 * Source: http://unix.stackexchange.com/a/49066
 *
 */

class Loader {
private:
  enum {
    MAGIC = 1234567890,
    MAJORVERSION = 2,
    MINORVERSION = 0,
    BUILDVERSION = 0,

    // TC prefix stands for 'typecode'
    TC_OPERATION = -128,
    TC_INT       = 0,
    TC_STRING    = 2,
    TC_DOUBLE    = 5,
    TC_TRUE      = 8,
    TC_FALSE     = 9,
    TC_NULL      = 10
  };

  unsigned    read_uint(std::ifstream &file, int size);
  std::string read_string(std::ifstream &file);
  char        read_char(std::ifstream &file);
  double      read_double(std::ifstream &file);
  void        read_operation(std::ifstream &file);

public:
  Loader();

  void load(std::string filename);
};

#endif // LOADER_H
