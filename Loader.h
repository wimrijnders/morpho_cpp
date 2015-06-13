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
 */

class Loader {
private:
  unsigned read_uint32(std::ifstream &file);

public:
  Loader();

  void load(std::string filename);
};

#endif // LOADER_H
