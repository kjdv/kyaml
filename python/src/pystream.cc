#include "pystream.hh"

using namespace std;
using namespace pykyaml;

namespace
{
  class py_filereader : public py_reader
  {
  public:
    static bool check(py_object const &object)
    {
      return PyFile_Check(object.get());
    }

    py_filereader(py_object &&object) :
      d_self(std::move(object))
    {
      assert(check(d_self));
    }

    int read(char *buf, size_t n) override
    {
      file_guard guard(d_self);

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
    class file_guard
    {
    public:
      file_guard(py_object &self) :
        d_self((PyFileObject *)self.get())
      {
        PyFile_IncUseCount(d_self);
      }

      ~file_guard()
      {
        PyFile_DecUseCount(d_self);
      }

    private:
      PyFileObject *d_self;
    };

    py_object d_self;
  };

  class py_objectreader : public py_reader
  {
  public:
    static bool check(py_object const &object)
    {
      return PyObject_HasAttrString(const_cast<PyObject *>(object.get()), "read");
    }

    py_objectreader(py_object &&object) :
      d_self(std::move(object))
    {
      assert(check(d_self));
    }

    int read(char *buf, size_t n) override
    {
      py_object result(PyObject_CallMethod(d_self.get(), (char *)"read", (char *)"I", n), false);
      if(result.get())
      {
        if(PyByteArray_Check(result.get()))
          return py_read(PyByteArray_Size, PyByteArray_AsString, result.get(), buf, n);
        else if(PyString_Check(result.get()))
          return py_read(PyString_Size, PyString_AsString, result.get(), buf, n);
      }
      return EOF;
    }

  private:
    Py_ssize_t py_read(Py_ssize_t (*size_f)(PyObject *),
                       char *(*string_f)(PyObject *),
                       PyObject *o, char *buf, Py_ssize_t n)
    {
      Py_ssize_t r = std::min(n, size_f(o));
      char *source = string_f(o);
      std::copy(source, source + r, buf);
      return r;
    }

    py_object d_self;
  };
}

bool py_reader::check(py_object const &object)
{
  return
    py_filereader::check(object) ||
    py_objectreader::check(object);
}

unique_ptr<py_reader> py_reader::create(py_object &&object)
{
  if(py_filereader::check(object))
    return unique_ptr<py_reader>(new py_filereader(std::move(object)));
  else if(py_objectreader::check(object))
    return unique_ptr<py_reader>(new py_objectreader(std::move(object)));

  return unique_ptr<py_reader>();
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
    setg(d_buffer, d_buffer, d_buffer + r);
    return *gptr();
  }
  return EOF;
}
