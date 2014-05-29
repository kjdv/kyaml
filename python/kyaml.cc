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

extern "C"
{
  PyMODINIT_FUNC initpykyaml()
  {
    PyObject *module = Py_InitModule("kyaml", module_methods);

    if(!module)
      return;

    PyModule_AddIntConstant(module, "ONE", 1);

  }
}
