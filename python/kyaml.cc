#include <Python.h>
#include "kyaml.hh"

namespace
{
  PyObject *dummy(PyObject *self, PyObject *arg)
  {
    return NULL;
  }

  PyMethodDef module_methods[] =
  {
    {"dummy", (PyCFunction)dummy, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
  };
}

PyMODINIT_FUNC initpykyaml()
{
  Py_InitModule3("kyaml", module_methods, "");
}
