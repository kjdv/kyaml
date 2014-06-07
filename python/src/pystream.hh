#ifndef PYSTREAM_HH
#define PYSTREAM_HH

#include <streambuf>
#include <istream>

#include "pykyaml.hh"

// use a python file object as a std::istream, how cool
namespace pykyaml
{
  class py_istreambuf : public std::streambuf
  {
  public:
    static bool check(py_object const &object);

    py_istreambuf(py_object &&object);

    ~py_istreambuf();

    int underflow() override;

  private:
    py_istreambuf(py_istreambuf const &); // NI
    py_istreambuf &operator=(py_istreambuf const &); // NI

    enum {bufsize = 1};
    char d_buffer[bufsize];

    py_object d_self;
  };

  class py_istream : public std::istream
  {
  public:
    py_istream(py_object &&object) :
      std::istream(nullptr),
      d_buf(std::move(object))
    {
      // note: this construct kinda assumes the buffer is not used
      // during istream's ctor or dtor
      rdbuf(&d_buf);
    }

    static bool check(py_object const &object)
    {
      return py_istreambuf::check(object);
    }

  private:
    py_istreambuf d_buf;
  };
}

#endif // PYSTREAM
