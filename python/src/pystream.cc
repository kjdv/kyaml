#include "pystream.hh"
#include "utils.hh"

using namespace std;
using namespace pykyaml;

bool py_istreambuf::check(py_object const &object)
{
  return PyFile_Check(object.get());
}

py_istreambuf::py_istreambuf(py_object const &object) :
  d_self(object)
{
  assert(check(d_self));
  PyFile_IncUseCount((PyFileObject *)d_self.get());
}

py_istreambuf::~py_istreambuf()
{
  PyFile_DecUseCount((PyFileObject *)d_self.get());
}

int py_istreambuf::underflow()
{
  FILE *fp = PyFile_AsFile(d_self.get());
  if(fp)
  {
    size_t r = fread(d_buffer, sizeof(char), bufsize, fp);
    if(r > 0)
    {
      setg(d_buffer, d_buffer, d_buffer + bufsize);
      return *gptr();
    }
  }
  return EOF;
}
