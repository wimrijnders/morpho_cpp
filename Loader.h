#ifndef LOADER_H
#define LOADER_H
#include <fstream>
#include <string>
#include <vector>

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

  bool load(std::ifstream &file);

  /**
   * @brief Representation of assembly code, for display purposes.
   */
  class CodeDump {
  public:
    class DumpItem {
    public: //private:
      bool m_has_offset{false};
      int  m_offset{0};
      std::string m_line;
      int m_copy_count{0};
      int m_copied_from{-1};

      std::vector<std::string> m_params;

    public:
      void offset(int val);
      void line(const std::string &val) { m_line = val; }
      void param(const std::string &val) { m_params.push_back(val); }

      std::string display(unsigned op_count, unsigned name_size);
    };

  private:
    std::vector<DumpItem> m_list;

  public:
    void add(DumpItem &item) { m_list.push_back(item); }
    void copy(int val);
    void display();
  } m_code_dump;

public:
  Loader();

  void load(std::string filename);
};

#endif // LOADER_H
