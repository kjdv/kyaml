#include "pystream.hh"

using namespace std;
using namespace pykyaml;

namespace
{
  class py_filereader : public py_reader
  {
  public:
    py_filereader(py_object &&object) :
      d_self(std::move(object))
    {
      assert(PyFile_Check(d_self.get()));
      PyFile_IncUseCount((PyFileObject *)d_self.get());
    }

    ~py_filereader()
    {
      PyFile_DecUseCount((PyFileObject *)d_self.get());
    }

    int read(char *buf, size_t n) override
    {
      FILE *fp = PyFile_AsFile(d_self.get());
      if(fp)
      {
        size_t r = fread(buf, sizeof(char), n, fp);
        if(r > 0)
          return r;
      }
      return EOF;
    }

  private:
    py_object d_self;
  };
}

bool py_reader::check(py_object const &object)
{
  return PyFile_Check(object.get());
}

unique_ptr<py_reader> py_reader::create(py_object &&object)
{
  return unique_ptr<py_reader>(new py_filereader(std::move(object)));
}

py_istreambuf::py_istreambuf(py_object &&object) :
  d_reader(py_reader::create(std::move(object)))
{
  assert(d_reader);
}

int py_istreambuf::underflow()
{
  assert(d_reader);

  int r = d_reader->read(d_buffer, bufsize);
  if(r > 0)
  {
    setg(d_buffer, d_buffer, d_buffer + bufsize);
    return *gptr();
  }
  return EOF;
}
