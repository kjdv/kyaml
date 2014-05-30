#include "pykyaml.hh"
#include "pystream.hh"

using namespace std;
using namespace pykyaml;

PyObject *call_checker::s_exception = nullptr;

namespace
{
  PyObject *throw_delegate(int a, int b)
  {
    if(a < b)
      throw value_error("a < b");
    return nullptr;
  }

  PyObject *thrower(PyObject *self, PyObject *arg)
  {
    return call_checker().call(throw_delegate, 5, 6);
  }

  PyObject *readline(PyObject *self, PyObject *arg)
  {
    if(!py_istream::check(arg))
    {
      PyErr_BadArgument();
      return NULL;
    }

    py_istream stream(arg);
    string line;
    getline(stream, line);

    PyObject *result = PyString_FromStringAndSize(line.data(), line.size());
    return result;
  }

  PyMethodDef module_methods[] =
  {
    // todo: only for testing/ development purposes. maybe find a clear way to mark it as such, or remove
    {"readline", (PyCFunction)readline, METH_O, ""},
    {"throw",    (PyCFunction)thrower,  METH_NOARGS, ""},

    {nullptr, nullptr, 0, nullptr}
  };
}

PyMODINIT_FUNC initpykyaml(void)
{
  PyObject *module = Py_InitModule("pykyaml", module_methods);

  if(!module)
    return;

  call_checker::initialize("kyaml.error");
  PyModule_AddObject(module, "error", call_checker::get());

  init_parser(module);
  init_leaf(module);
}
